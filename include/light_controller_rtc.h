#pragma once

#include <initializer_list>
#include <etl/vector.h>
#include <assert.h>
#include "project_types.h"
#include "rtc_time_container.h"
#include "light_state.h"

namespace LightControl
{

class DaytimeEvent
{
public:
    DaytimeEvent();
    explicit DaytimeEvent(const ProjectTypes::abs_min_past_midnight_t &event_time, const ProjectTypes::time_minute_diff_t &time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t &time_to_turn_off_after_event );

    /*
        @brief: Checks if event is active.
        @param: current_rtc_time - current time in RTC in absolute minutes past midnight.
        @return: true if event is active, false otherwise.
    */
    bool checkEventIsActive(const ProjectTypes::abs_min_past_midnight_t & current_time) const;
    /*
        @brief: Sets new event time.
        @param: new_event_time - new event time in absolute minutes past midnight.
    */
    void setEventTime(const ProjectTypes::abs_min_past_midnight_t &new_event_time);
    /*
        @brief: Get event time.
        @return: new_event_time - new event time in absolute minutes past midnight.
    */
    ProjectTypes::abs_min_past_midnight_t getEventTime() const;
    /*
        @brief: Sets new time to turn on before event.
        @param: new_time_to_turn_on_before_event - new time to turn on before event in minutes.
    */
    void setNewTimeToTurnOnBeforeEvent(const ProjectTypes::time_minute_diff_t &new_time_to_turn_on_before_event);
    /*
        @brief: Sets new time to turn off after event.
        @param: new_time_to_turn_off_after_event - new time to turn off after event in minutes.
    */
    void setNewTimeToTurnOffAfterEvent(const ProjectTypes::time_minute_diff_t &new_time_to_turn_off_after_event);
    /*
        @brief: get value of time when event will be active before happen
        @return: time to turn on before event in minutes
    */
    ProjectTypes::time_minute_diff_t getTimeToTurnOnBeforeEvent() const;
    /*
        @brief: get value of time when event will be active after happen
        @return: time to turn on before event in minutes
    */
    ProjectTypes::time_minute_diff_t getTimeToTurnOffAfterEvent() const;
    /*
        @brief: get time past event
        @param: current_rtc_time - current time in RTC in absolute minutes past midnight.
        @return: time past event in minutes
    */
    ProjectTypes::time_minute_diff_t getTimePastEvent(const ProjectTypes::abs_min_past_midnight_t &current_time) const;
    /*
        @brief: get time to event
        @param: current_rtc_time - current time in RTC in absolute minutes past midnight.
        @return: time to event in minutes
    */
    ProjectTypes::time_minute_diff_t getTimeToEvent(const ProjectTypes::abs_min_past_midnight_t &current_time) const;

private:
    ProjectTypes::abs_min_past_midnight_t event_time_;
    ProjectTypes::time_minute_diff_t time_to_turn_on_before_event_;
    ProjectTypes::time_minute_diff_t time_to_turn_off_after_event_;

    static constexpr ProjectTypes::time_minute_diff_t kTimeInitValue = 0;
    static constexpr ProjectTypes::time_minute_diff_t kMaxAbsValueInDay = 24*60;

    bool checkTimeIsInRange(const ProjectTypes::time_minute_diff_t &relative_time) const;

    // TODO : refactor this code
    static ProjectTypes::abs_min_past_midnight_t getAbsMinPastMidnightFromRtc(const ProjectTypes::RTC_Time &rtc_time);
};

class LightEvent
{
public:
    LightEvent(const ProjectTypes::abs_min_past_midnight_t &event_time, const ProjectTypes::time_minute_diff_t &time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t &time_to_turn_off_after_event, const ProjectTypes::time_minute_diff_t &time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t &time_to_end_blanking_after_turn_off);
    /*
        @note: check if light state should change to blanking or turn on, turn on have priority over blanking.
        @param: abs_rtc_time - current absolute time in RTC in absolute minutes past midnight.
        @return: light state to set.
    */
    LightControl::LightState checkLightState(const ProjectTypes::abs_min_past_midnight_t & abs_rtc_time);
    /*
        @brief: Sets new event time.
        @param: new_event_time - new event time in absolute minutes past midnight.
    */
    void setEventTime(const ProjectTypes::abs_min_past_midnight_t &new_event_time);
    /*
        @brief: set new event detection time
        @param: time_to_turn_on_before_event - time to turn on before event in minutes
        @param: time_to_turn_off_after_event - time to turn off after event in minutes
        @param: time_to_start_blanking_before_turn_on - time to start blanking before turn on in minutes
        @param: time_to_end_blanking_after_turn_off - time to end blanking after turn off in minutes
    */
    void setNewActivationAndBlankingTime(const ProjectTypes::time_minute_diff_t &time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t &time_to_turn_off_after_event, const ProjectTypes::time_minute_diff_t &time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t &time_to_end_blanking_after_turn_off);
    /*
        @brief: set new detection blanking time
        @param: time_to_start_blanking_before_turn_on - time to start blanking before turn on in minutes
        @param: time_to_end_blanking_after_turn_off - time to end blanking after turn off in minutes
    */
    void setNewBlankingTime(const ProjectTypes::time_minute_diff_t &time_to_start_blanking_before_turn_on, const ProjectTypes::time_minute_diff_t &time_to_end_blanking_after_turn_off);
    /*
        @brief: set new detection time
        @param: time_to_turn_on_before_event - time to turn on before event in minutes
    */
    void setNewActivationTime(const ProjectTypes::time_minute_diff_t &time_to_turn_on_before_event, const ProjectTypes::time_minute_diff_t &time_to_turn_off_after_event);
    /*
        @brief: calculate rest of blanking time in blanking mode
        @param: abs_rtc_time - current absolute time in RTC in absolute minutes past midnight.
        @return: rest of blanking time in minutes
    */
    ProjectTypes::abs_min_past_midnight_t getRestBlankingTime(const ProjectTypes::abs_min_past_midnight_t &abs_rtc_time, ProjectTypes::abs_min_past_midnight_t *total_blanking_time_ptr = nullptr);
    /*
        @brief: calculate rest of blanking time in blanking mode in percent
        @param: abs_rtc_time - current absolute time in RTC in absolute minutes past midnight.
        @return: rest of blanking time in percent [0.0 - 1.0]
    */
    float getRestBlankingTimePercent(const ProjectTypes::abs_min_past_midnight_t &abs_rtc_time);

private:
    DaytimeEvent turn_on_event_;
    DaytimeEvent start_blanking_event_;
    LightState light_state_;
    ProjectTypes::time_minute_diff_t time_to_turn_off_after_event_;
    ProjectTypes::time_minute_diff_t time_to_turn_on_before_event_;
    ProjectTypes::time_minute_diff_t time_to_start_blanking_before_turn_on_;
    ProjectTypes::time_minute_diff_t time_to_end_blanking_after_turn_off_;

    void updateLightState(const ProjectTypes::abs_min_past_midnight_t &abs_rtc_time);

    void updateEventActivationAndBlankingTime();
};

struct LightEventAndUpdateCallback
{
    LightEvent event_;
    std::function<ProjectTypes::abs_min_past_midnight_t(const ProjectTypes::RTC_Time &)> callback_;
};

class LightControllerRTC
{
public:
    static constexpr size_t kMaxEventsCount = 10;

    LightControllerRTC();
    explicit LightControllerRTC(const etl::vector<LightEventAndUpdateCallback, kMaxEventsCount> &events_containers);

    LightControl::LightState getLightState(const ProjectTypes::RTC_Time &rtc_time);

    bool updateEventTime(const ProjectTypes::abs_min_past_midnight_t &new_event_time, const size_t event_index);

    bool updateBlankingTime(const ProjectTypes::abs_min_past_midnight_t &new_blanking_time, const size_t event_index);

    bool addEvent(const LightEventAndUpdateCallback &new_event);

    ProjectTypes::abs_min_past_midnight_t getRestOfBlankingTime(const ProjectTypes::RTC_Time & rtc_time);

    float getRestOfBlankingTimePercent(const ProjectTypes::RTC_Time & rtc_time) const;

    void updateEvents(const ProjectTypes::RTC_Time & rtc_time);

private:
    mutable etl::vector<LightEventAndUpdateCallback, kMaxEventsCount> event_containers_;

}; // class LightControllerRTC

} // namespace LightControl
