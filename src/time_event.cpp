#include "time_event.h"

static constexpr uint32_t kSecondPerMinute = 60U;

// Global functions
int64_t convertMinutesToSeconds(const ProjectTypes::time_minute_t & time) {
    return static_cast<int64_t>(time) * kSecondPerMinute;
}

int64_t convertSecondsToMinutes(const std::time_t & time) {
    return time / kSecondPerMinute;
}

// TimeEvent class implementation
TimeEvent::TimeEvent():
    start_event_time_(kTimeInitValue),
    stop_event_time_(kTimeInitValue) {
}

TimeEvent::TimeEvent(const std::time_t & start_event_time,
                     const std::time_t & stop_event_time):
    start_event_time_(start_event_time),
    stop_event_time_(stop_event_time) {
}

bool TimeEvent::checkEventIsActive(const std::time_t & current_time) const {
    return current_time > start_event_time_ && current_time < stop_event_time_;
}

void TimeEvent::setEventTime(const std::time_t & start_event_time,
                             const std::time_t & stop_event_time) {
    start_event_time_ = start_event_time;
    stop_event_time_ = stop_event_time;
}

std::time_t TimeEvent::getStartEventTime() const {
    return start_event_time_;
}

std::time_t TimeEvent::getStopEventTime() const {
    return stop_event_time_;
}

// TimestampEvent class implementation
TimestampEvent::TimestampEvent():
    event_time_(kTimeInitValue),
    time_to_turn_on_before_event_(kTimeInitValue),
    time_to_turn_off_after_event_(kTimeInitValue) {
}

TimestampEvent::TimestampEvent(const time_t & event_time,
                    const ProjectTypes::time_minute_t & time_to_turn_on_before_event,
                    const ProjectTypes::time_minute_t & time_to_turn_off_after_event):
        event_time_(event_time),
        time_to_turn_on_before_event_(time_to_turn_on_before_event),
        time_to_turn_off_after_event_(time_to_turn_off_after_event) {
    updateEventTime();
}

void TimestampEvent::setEventTime(const std::time_t & new_event_time) {
    event_time_ = new_event_time;
    updateEventTime();
}

std::time_t TimestampEvent::getEventTime() const {
    return event_time_;
}

void TimestampEvent::setNewTimeToTurnOnBeforeEvent(const ProjectTypes::time_minute_t & new_time_to_turn_on_before_event) {
    time_to_turn_on_before_event_ = new_time_to_turn_on_before_event;
    updateEventTurnOnTime();
}

void TimestampEvent::setNewTimeToTurnOffAfterEvent(const ProjectTypes::time_minute_t & new_time_to_turn_off_after_event) {
    time_to_turn_off_after_event_ = new_time_to_turn_off_after_event;
    updateEventTurnOffTime();
}

ProjectTypes::time_minute_t TimestampEvent::getTimeToTurnOnBeforeEvent() const {
    return time_to_turn_on_before_event_;
}

ProjectTypes::time_minute_t TimestampEvent::getTimeToTurnOffAfterEvent() const {
    return  time_to_turn_off_after_event_;
}

ProjectTypes::time_minute_t TimestampEvent::getTimeToEvent(const std::time_t & current_time) const {
    return static_cast<ProjectTypes::time_minute_t>(std::difftime(start_event_time_, current_time));
}

bool TimestampEvent::checkEventWasHappened(const std::time_t & current_time) const {
    return current_time > event_time_;
}

ProjectTypes::time_minute_t TimestampEvent::getTimePastEvent(const std::time_t & current_time) const {
    return static_cast<ProjectTypes::time_minute_t>(std::difftime(current_time, stop_event_time_));
}

void TimestampEvent::updateEventTurnOffTime() {
    stop_event_time_ = event_time_ + convertMinutesToSeconds(time_to_turn_off_after_event_);
}

void TimestampEvent::updateEventTurnOnTime() {
    start_event_time_ = event_time_ - convertMinutesToSeconds(time_to_turn_on_before_event_);
}

void TimestampEvent::updateEventTime() {
    updateEventTurnOffTime();
    updateEventTurnOnTime();
}
