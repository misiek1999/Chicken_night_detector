#pragma once

#include <ctime>
#include <cinttypes>
#include "project_types.h"


// Global functions
int64_t convertMinutesToSeconds(const ProjectTypes::time_minute_t & time);
int64_t convertSecondsToMinutes(const std::time_t & time);


class TimeEvent {
public:
    TimeEvent();
    explicit TimeEvent(const std::time_t &start_event_time,
                       const std::time_t &stop_event_time);

    /*
        @brief: Checks if event is active.
        @param: current_rtc_time - current time
        @return: true if event is active, false otherwise.
    */
    bool checkEventIsActive(const std::time_t & current_time) const;

    /*
        @brief: Sets new start and stop event time.
        @param: start_event_time - new start event time
        @param: stop_event_time - new stop event time
    */
    void setEventTime(const std::time_t &start_event_time, const std::time_t &stop_event_time);

    /*
        @brief: Get start event time.
        @return: start event time
    */
    std::time_t getStartEventTime() const;

    /*
        @brief: Get stop event time.
        @return: stop event time
    */
    std::time_t getStopEventTime() const;

protected:
    std::time_t start_event_time_;
    std::time_t stop_event_time_;
    static constexpr std::time_t kTimeInitValue = {};
};

class TimestampEvent : public TimeEvent{
public:
    TimestampEvent();
    explicit TimestampEvent(const std::time_t &event_time,
                          const ProjectTypes::time_minute_t &time_to_turn_on_before_event,
                          const ProjectTypes::time_minute_t &time_to_turn_off_after_event);

    // use method from base class
    using TimeEvent::checkEventIsActive;

    // remove setEventTime from base class
    void setEventTime(const std::time_t &start_event_time, const std::time_t &stop_event_time) = delete;

    /*
        @brief: Sets new event time.
        @param: new_event_time - new event time
    */
    void setEventTime(const std::time_t &new_event_time);

    /*
        @brief: Get event time.
        @return: new_event_time - new event time
    */
    std::time_t getEventTime() const;

    /*
        @brief: Sets new time to turn on before event.
        @param: new_time_to_turn_on_before_event - new time to turn on before event in minutes.
    */
    void setNewTimeToTurnOnBeforeEvent(const ProjectTypes::time_minute_t &new_time_to_turn_on_before_event);

    /*
        @brief: Sets new time to turn off after event.
        @param: new_time_to_turn_off_after_event - new time to turn off after event in minutes.
    */
    void setNewTimeToTurnOffAfterEvent(const ProjectTypes::time_minute_t &new_time_to_turn_off_after_event);

    /*
        @brief: get value of time when event will be active before happen
        @return: time to turn on before event in minutes
    */
    ProjectTypes::time_minute_t getTimeToTurnOnBeforeEvent() const;

    /*
        @brief: get value of time when event will be active after happen
        @return: time to turn on before event in minutes
    */
    ProjectTypes::time_minute_t getTimeToTurnOffAfterEvent() const;

    /*
        @brief: get time past event
        @param: current_rtc_time - current time
        @return: time past event in seconds
    */
    ProjectTypes::time_minute_t getTimePastEvent(const std::time_t &current_time) const;

    /*
        @brief: get time to event
        @param: current_rtc_time - current time
        @return: time to event in seconds
    */
    ProjectTypes::time_minute_t getTimeToEvent(const std::time_t &current_time) const;

    /*
        @brief: check event was happened
        @param: current_time
        @return: true if event was happened, false otherwise
    */
    bool checkEventWasHappened(const std::time_t &current_time) const;

    using TimeEvent::getStartEventTime;
    using TimeEvent::getStopEventTime;

private:
    std::time_t event_time_;
    ProjectTypes::time_minute_t time_to_turn_on_before_event_;
    ProjectTypes::time_minute_t time_to_turn_off_after_event_;

    void updateEventTurnOnTime();
    void updateEventTurnOffTime();
    void updateEventTime();
};
