#include "light_controller_rtc.h"
#include "calculate_sunset_and_sunrise_time.h"

//TODO: refactor below code
constexpr ProjectTypes::abs_min_past_midnight_t kMinPerHour = 60;
constexpr ProjectTypes::abs_min_past_midnight_t kMinPerDay = 24*60;

ProjectTypes::time_minute_diff_t get_time_difference_minute(ProjectTypes::abs_min_past_midnight_t time1, ProjectTypes::abs_min_past_midnight_t time2)
{
    return static_cast<ProjectTypes::time_minute_diff_t>(time1) - static_cast<ProjectTypes::time_minute_diff_t>(time2);
}

ProjectTypes::time_minute_diff_t format_time_to_24h(ProjectTypes::time_minute_diff_t time)
{
    ProjectTypes::time_minute_diff_t time_24h = time;
    if (time < 0)
    {
        time_24h += kMinPerDay;
    }
    else if (time > ProjectTypes::time_minute_diff_t(kMinPerDay))
    {
        time_24h -= kMinPerDay;
    }
    return time_24h;
}

ProjectTypes::abs_min_past_midnight_t getAbsMinPastMidnightFromRtc(const ProjectTypes::RTC_Time & rtc_time)
{
    return rtc_time.min + rtc_time.hr * kMinPerHour;
}


// DaytimeEvent class implementation
LightControl::DaytimeEvent::DaytimeEvent():
    event_time_(static_cast<ProjectTypes::abs_min_past_midnight_t>(kTimeInitValue)),
    time_to_turn_on_before_event_(kTimeInitValue),
    time_to_turn_off_after_event_(kTimeInitValue)
{
}

LightControl::DaytimeEvent::DaytimeEvent(const ProjectTypes::abs_min_past_midnight_t & event_time, const ProjectTypes::time_minute_diff_t & time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t & time_to_turn_off_after_event):
    event_time_(event_time),
    time_to_turn_on_before_event_(time_to_turn_on_before_event),
    time_to_turn_off_after_event_(time_to_turn_off_after_event)
{
}

bool LightControl::DaytimeEvent::checkEventIsActive(const ProjectTypes::abs_min_past_midnight_t & current_time) const
{
    bool is_active = false;
    // convert absolute time format to relative
    ProjectTypes::time_minute_diff_t relative_time = static_cast<ProjectTypes::time_minute_diff_t>(current_time);
    // normal case || time is after midnight || time is before midnight
    if (checkTimeIsInRange(relative_time) || checkTimeIsInRange(relative_time + kMaxAbsValueInDay) || checkTimeIsInRange(relative_time - kMaxAbsValueInDay))
    {
        is_active = true;
    }
    return is_active;
}

void LightControl::DaytimeEvent::setEventTime(const ProjectTypes::abs_min_past_midnight_t & new_event_time)
{
    event_time_ = new_event_time;
}

ProjectTypes::abs_min_past_midnight_t LightControl::DaytimeEvent::getEventTime() const
{
    return event_time_;
}

void LightControl::DaytimeEvent::setNewTimeToTurnOnBeforeEvent(const ProjectTypes::time_minute_diff_t & new_time_to_turn_on_before_event)
{
    time_to_turn_on_before_event_ = new_time_to_turn_on_before_event;
}

void LightControl::DaytimeEvent::setNewTimeToTurnOffAfterEvent(const ProjectTypes::time_minute_diff_t & new_time_to_turn_off_after_event)
{
    time_to_turn_off_after_event_ = new_time_to_turn_off_after_event;
}

ProjectTypes::time_minute_diff_t LightControl::DaytimeEvent::getTimeToTurnOnBeforeEvent() const
{
    return time_to_turn_on_before_event_;
}

ProjectTypes::time_minute_diff_t LightControl::DaytimeEvent::getTimeToEvent(const ProjectTypes::abs_min_past_midnight_t & current_time) const
{
    ProjectTypes::time_minute_diff_t time_diff = get_time_difference_minute(event_time_, current_time);
    return format_time_to_24h(time_diff);
}
ProjectTypes::time_minute_diff_t LightControl::DaytimeEvent::getTimeToTurnOffAfterEvent() const
{
    return time_to_turn_off_after_event_;
}

ProjectTypes::time_minute_diff_t LightControl::DaytimeEvent::getTimePastEvent(const ProjectTypes::abs_min_past_midnight_t & current_time) const
{
    ProjectTypes::time_minute_diff_t time_diff = get_time_difference_minute(current_time, event_time_);
    return format_time_to_24h(time_diff);
}

bool LightControl::DaytimeEvent::checkTimeIsInRange(const ProjectTypes::time_minute_diff_t & relative_time) const
{
    bool is_in_range = false;
    ProjectTypes::time_minute_diff_t event_time_relative = static_cast<ProjectTypes::time_minute_diff_t>(event_time_);
    if (relative_time + time_to_turn_on_before_event_ > event_time_relative &&
        relative_time < event_time_relative + time_to_turn_off_after_event_ )
    {
        is_in_range = true;
    }
    return is_in_range;
}

// LightEvent class implementation
LightControl::LightEvent::LightEvent(const ProjectTypes::abs_min_past_midnight_t &event_time, const ProjectTypes::time_minute_diff_t &time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t &time_to_turn_off_after_event, const ProjectTypes::time_minute_diff_t &time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t &time_to_end_blanking_after_turn_off):
     turn_on_event_(event_time, time_to_turn_on_before_event, time_to_turn_off_after_event),
    start_blanking_event_(event_time, time_to_start_blanking_before_turn_on + time_to_turn_on_before_event, time_to_end_blanking_after_turn_off + time_to_turn_off_after_event),
    light_state_(LightState::Off),
    time_to_turn_off_after_event_(time_to_turn_off_after_event),
    time_to_turn_on_before_event_(time_to_turn_on_before_event),
    time_to_start_blanking_before_turn_on_(time_to_start_blanking_before_turn_on),
    time_to_end_blanking_after_turn_off_(time_to_end_blanking_after_turn_off)
{
}

LightControl::LightState LightControl::LightEvent::checkLightState(const ProjectTypes::abs_min_past_midnight_t & abs_rtc_time)
{
    updateLightState(abs_rtc_time);
    return light_state_;
}

void LightControl::LightEvent::setEventTime(const ProjectTypes::abs_min_past_midnight_t & new_event_time)
{
    turn_on_event_.setEventTime(new_event_time);
    start_blanking_event_.setEventTime(new_event_time);
}

void LightControl::LightEvent::setNewActivationAndBlankingTime(const ProjectTypes::time_minute_diff_t & time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t & time_to_turn_off_after_event, const ProjectTypes::time_minute_diff_t & time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t & time_to_end_blanking_after_turn_off)
{
    time_to_turn_on_before_event_ = time_to_turn_on_before_event;
    time_to_turn_off_after_event_ = time_to_turn_off_after_event;
    time_to_start_blanking_before_turn_on_ = time_to_start_blanking_before_turn_on;
    time_to_end_blanking_after_turn_off_ = time_to_end_blanking_after_turn_off;
    updateEventActivationAndBlankingTime();
}

void LightControl::LightEvent::setNewBlankingTime(const ProjectTypes::time_minute_diff_t & time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t & time_to_end_blanking_after_turn_off)
{
    time_to_start_blanking_before_turn_on_ = time_to_start_blanking_before_turn_on;
    time_to_end_blanking_after_turn_off_ = time_to_end_blanking_after_turn_off;
    updateEventActivationAndBlankingTime();
}

void LightControl::LightEvent::setNewActivationTime(const ProjectTypes::time_minute_diff_t & time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t & time_to_turn_off_after_event)
{
    time_to_turn_on_before_event_ = time_to_turn_on_before_event;
    time_to_turn_off_after_event_ = time_to_turn_off_after_event;
    updateEventActivationAndBlankingTime();
}

// TODO:    1. Fix detect event was happen
//          2. Create local variable with blanking time to reduce calculation
ProjectTypes::abs_min_past_midnight_t LightControl::LightEvent::getRestBlankingTime(const ProjectTypes::abs_min_past_midnight_t & abs_rtc_time, ProjectTypes::abs_min_past_midnight_t *total_blanking_time_ptr)
{
    ProjectTypes::abs_min_past_midnight_t total_blanking_time = 0;
    ProjectTypes::abs_min_past_midnight_t rest_blanking_time = 0;
    updateLightState(abs_rtc_time);
    // check light state is in Blanking mode
    if (light_state_ == LightState::Blanking)
    {
        ProjectTypes::time_minute_diff_t time_past_event = start_blanking_event_.getTimePastEvent(abs_rtc_time);
        ProjectTypes::time_minute_diff_t time_before_event = start_blanking_event_.getTimeToEvent(abs_rtc_time);
        // check event was happen
        if (time_past_event < time_before_event)
        {
            // case when event was happen and we should start blanking after turn off
            ProjectTypes::time_minute_diff_t time_diff = get_time_difference_minute( start_blanking_event_.getEventTime() + start_blanking_event_.getTimeToTurnOffAfterEvent(), abs_rtc_time);
            time_diff = format_time_to_24h(time_diff);
            rest_blanking_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(time_diff);
            total_blanking_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(get_time_difference_minute(start_blanking_event_.getTimeToTurnOffAfterEvent(), turn_on_event_.getTimeToTurnOffAfterEvent()));
        }
        else
        {
            // case when event will be happen and we should start blanking before turn on
            ProjectTypes::time_minute_diff_t time_diff = get_time_difference_minute(start_blanking_event_.getEventTime(), abs_rtc_time + turn_on_event_.getTimeToTurnOnBeforeEvent());
            time_diff = format_time_to_24h(time_diff);
            rest_blanking_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(time_diff);
            total_blanking_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(get_time_difference_minute(start_blanking_event_.getTimeToTurnOnBeforeEvent(), turn_on_event_.getTimeToTurnOnBeforeEvent()));
        }
    }
    if (total_blanking_time_ptr!= nullptr)
    {
        *total_blanking_time_ptr = total_blanking_time;
    }
    return rest_blanking_time;
}

float LightControl::LightEvent::getRestBlankingTimePercent(const ProjectTypes::abs_min_past_midnight_t & abs_rtc_time)
{
    ProjectTypes::abs_min_past_midnight_t total_blanking_time;
    ProjectTypes::abs_min_past_midnight_t rest_blanking_time = getRestBlankingTime(abs_rtc_time, &total_blanking_time);
    float percent = static_cast<float>(rest_blanking_time) / static_cast<float>(total_blanking_time);
    return percent;
}

void LightControl::LightEvent::updateLightState(const ProjectTypes::abs_min_past_midnight_t & abs_rtc_time)
{
    if (turn_on_event_.checkEventIsActive(abs_rtc_time))
    {
        light_state_ = LightState::On;
    }
    else if (start_blanking_event_.checkEventIsActive(abs_rtc_time))
    {
        light_state_ = LightState::Blanking;
    }
    else
    {
        light_state_ = LightState::Off;
    }
}

void LightControl::LightEvent::updateEventActivationAndBlankingTime()
{
    turn_on_event_.setNewTimeToTurnOffAfterEvent(time_to_turn_off_after_event_);
    turn_on_event_.setNewTimeToTurnOnBeforeEvent(time_to_turn_on_before_event_);
    start_blanking_event_.setNewTimeToTurnOffAfterEvent(time_to_end_blanking_after_turn_off_ + time_to_turn_off_after_event_);
    start_blanking_event_.setNewTimeToTurnOnBeforeEvent(time_to_start_blanking_before_turn_on_ + time_to_turn_on_before_event_);
}

// LightControllerRTC class implementation
LightControl::LightControllerRTC::LightControllerRTC()
{
}

LightControl::LightControllerRTC::LightControllerRTC(const etl::vector<LightEventAndUpdateCallback,kMaxEventsCount>& events_containers):
    event_containers_(events_containers)
{
}


LightControl::LightState LightControl::LightControllerRTC::getLightState(const ProjectTypes::RTC_Time & rtc_time)
{
    // default return state, should never return from this method
    LightControl::LightState light_state = LightControl::LightState::Undefined;
    // get abs min time past midnight from rtc time
    ProjectTypes::abs_min_past_midnight_t rtc_abs_min = getAbsMinPastMidnightFromRtc(rtc_time);
    // update events
    updateEvents(rtc_time);
    // check if light should start blinking
    for (auto event_container= event_containers_.begin(); event_container != event_containers_.end(); ++event_container)
    {
        light_state = event_container->event_.checkLightState(rtc_abs_min);
        if (light_state != LightControl::LightState::Off)
        {
            break;
        }
    }
    return light_state;
}

bool LightControl::LightControllerRTC::updateEventTime(const ProjectTypes::abs_min_past_midnight_t & new_event_time, const size_t event_index)
{
    bool status = false;
    if (event_containers_.size() > event_index && event_index >= 0)
    {
        event_containers_[event_index].event_.setEventTime(new_event_time);
        status = true;
    }
    return status;
}

bool LightControl::LightControllerRTC::updateBlankingTime(const ProjectTypes::abs_min_past_midnight_t &new_blanking_time, const size_t event_index)
{
    bool status = false;
    if (event_containers_.size() > event_index && event_index >= 0)
    {
        event_containers_[event_index].event_.setNewBlankingTime(new_blanking_time, new_blanking_time);
        status = true;
    }
    return status;
}

bool LightControl::LightControllerRTC::addEvent(const LightEventAndUpdateCallback & new_event)
{
    bool status = false;
    if (!event_containers_.full())
    {
        status = true;
        event_containers_.push_back(new_event);
    }
    return status;
}

ProjectTypes::abs_min_past_midnight_t LightControl::LightControllerRTC::getRestOfBlankingTime(const ProjectTypes::RTC_Time & rtc_time)
{
    ProjectTypes::abs_min_past_midnight_t rest_of_blanking_time = 0.0F;
    // get abs min time past midnight from rtc time
    ProjectTypes::abs_min_past_midnight_t rtc_abs_min = getAbsMinPastMidnightFromRtc(rtc_time);
    for (auto& event_container : event_containers_)
    {
        if (event_container.event_.checkLightState(rtc_abs_min) == LightControl::LightState::Blanking)
        {
            rest_of_blanking_time = event_container.event_.getRestBlankingTime(rtc_abs_min);
            break;
        }
    }
    return rest_of_blanking_time;
}

float LightControl::LightControllerRTC::getRestOfBlankingTimePercent(const ProjectTypes::RTC_Time & rtc_time) const
{
    float rest_of_blanking_time_percent = 0.0F;
    // get abs min time past midnight from rtc time
    ProjectTypes::abs_min_past_midnight_t rtc_abs_min = getAbsMinPastMidnightFromRtc(rtc_time);
    for (auto event_container= event_containers_.begin(); event_container != event_containers_.end(); ++event_container)
    {
        if (event_container->event_.checkLightState(rtc_abs_min) == LightControl::LightState::Blanking)
        {
            rest_of_blanking_time_percent = event_container->event_.getRestBlankingTimePercent(rtc_abs_min);
            break;
        }
    }
    return rest_of_blanking_time_percent;
}

void LightControl::LightControllerRTC::updateEvents(const ProjectTypes::RTC_Time & rtc_time)
{
    for (auto event_container= event_containers_.begin(); event_container != event_containers_.end(); ++event_container)
    {
        if (event_container->callback_ != nullptr)
        {
            ProjectTypes::abs_min_past_midnight_t event_time = event_container->callback_(rtc_time);
            event_container->event_.setEventTime(event_time);
        }
    }
}
