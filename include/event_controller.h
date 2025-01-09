#pragma once

#include <initializer_list>
#include <cassert>
#include <ctime>
#include <etl/unordered_map.h>
#include <etl/vector.h>
#include "project_types.h"
#include "light_state.h"
#include "time_event.h"

namespace ControlLogic {

class LightDimmingEvent {
 public:
    /*
    @brief: Constructor to create light dimming event.

    |<-- time_to_start_dimming_before_turn_on -->|<-- time_to_turn_on_before_event -->|<-- event_time -->|<-- time_to_turn_off_after_event -->|<-- time_to_end_dimming_after_turn_off -->|
    @param: event_time - event time
    @param: time_to_turn_on_before_event - time to turn on before event in minutes
    @param: time_to_turn_off_after_event - time to turn off after event in minutes
    @param: time_to_start_dimming_before_turn_on - time to start dimming before turn on in minutes
    @param: time_to_end_dimming_after_turn_off - time to end dimming after turn off in minutes
    */
    LightDimmingEvent(const std::time_t &event_time,
               const ProjectTypes::time_minute_t &time_to_turn_on_before_event,
               const ProjectTypes::time_minute_t &time_to_turn_off_after_event,
               const ProjectTypes::time_minute_t &time_to_start_dimming_before_turn_on,
               const ProjectTypes::time_minute_t &time_to_end_dimming_after_turn_off);
    /*
        @note: get light state for current event
        @param: current_time - current absolute time
        @return: light state to set.
    */
    ControlLogic::LightState getLightState(const std::time_t &current_time) const;

    /*
        @brief: Sets new event time.
        @param: new_event_time - new event time
    */
    void setEventTime(const std::time_t &new_event_time);

    /*
        @brief: set new event detection time
        @param: time_to_turn_on_before_event - time to turn on before event in minutes
        @param: time_to_turn_off_after_event - time to turn off after event in minutes
        @param: time_to_start_dimming_before_turn_on - time to start dimming before turn on in minutes
        @param: time_to_end_dimming_after_turn_off - time to end dimming after turn off in minutes
    */
    void setNewActivationAndDimmingTime(const ProjectTypes::time_minute_t &time_to_turn_on_before_event,
                                        const ProjectTypes::time_minute_t &time_to_turn_off_after_event,
                                        const ProjectTypes::time_minute_t &time_to_start_dimming_before_turn_on,
                                        const ProjectTypes::time_minute_t &time_to_end_dimming_after_turn_off);

    /*
        @brief: set new detection dimming time
        @param: time_to_start_dimming_before_turn_on - time to start dimming before turn on in minutes
        @param: time_to_end_dimming_after_turn_off - time to end dimming after turn off in minutes
    */
    void setNewDimmingTime(const ProjectTypes::time_minute_t &time_to_start_dimming_before_turn_on,
                           const ProjectTypes::time_minute_t &time_to_end_dimming_after_turn_off);

    /*
        @brief: set new detection time
        @param: time_to_turn_on_before_event - time to turn on before event in minutes
    */
    void setNewActivationTime(const ProjectTypes::time_minute_t &time_to_turn_on_before_event,
                              const ProjectTypes::time_minute_t &time_to_turn_off_after_event);

    /*
        @brief: calculate rest of dimming time in dimming mode
        @param: current_time - current time
        @return: rest of dimming time in minutes
    */
    ProjectTypes::time_minute_t getRestLightDimmingTime(const std::time_t &current_time) const;

    /*
        @brief: calculate rest of dimming time in dimming mode in percent
        @param: current_time - current time
        @return: rest of dimming time in percent [0.0 - 1.0]
    */
    float getRestLightDimmingTimePercent(const std::time_t &current_time) const;

 private:
    TimestampEvent turn_on_event_;
    TimestampEvent start_dimming_event_;
    ProjectTypes::time_minute_t time_to_turn_off_after_event_;
    ProjectTypes::time_minute_t time_to_turn_on_before_event_;
    ProjectTypes::time_minute_t time_to_start_dimming_before_turn_on_;
    ProjectTypes::time_minute_t time_to_end_dimming_after_turn_off_;

    /*
        @brief: update event activation and dimming time
    */
    void updateEventActivationAndDimmingTime();

    /*
        @brief: get current dimming time
        @param: current_time - current time
        @return: current dimming time in minutes
    */
    ProjectTypes::time_minute_t getCurrentDimmingTime(const std::time_t &current_time) const;
};

//  Number of total events handled by the bulb light controller
struct LightDimmingEventAndCallback {
    LightDimmingEvent event;
    EventUpdateCallback callback;
};

static constexpr size_t kMaxEventsCount = 2;    // we dont need more than 2 events
using LightDimmingEventMap = etl::unordered_map<size_t, LightDimmingEventAndCallback, kMaxEventsCount>;
using LightStateMap = etl::unordered_map<size_t, ControlLogic::LightState, kMaxEventsCount>;
using RestDimmingTimeMap = etl::unordered_map<size_t, ProjectTypes::time_minute_t, kMaxEventsCount>;
using RestDimmingTimePercentMap = etl::unordered_map<size_t, float, kMaxEventsCount>;

/*
    * This class should be used to control signle instance of light bulb.
*/
class LightEventController {
 public:
    LightEventController() = default;

    /*
        @brief: Constructor to create light bulb controller.
        @param: events_containers - map of light dimming events and coresponding callbacks
    */
    explicit LightEventController(const LightDimmingEventMap &events_containers);

    /*
        @brief: get event state for selected event
        @param: current_time - current time
        @return: state for single instance of selected event
    */
    LightState getEventState(const std::time_t &current_time, const size_t event_index);

    /*
        @brief: get event state for all events
        @param: current_time - current time
        @return: etl vector of all event state
    */
    LightStateMap getAllEventStates(const std::time_t &current_time);

    /*
        @brief: get light state for current time
        @param: current_time - current time
        @return: light state to set.
    */
    LightState getLightState(const std::time_t &current_time);

    /*
        @brief: add new event to light controller
                fill event parameters with default values
        @param: new_event - new event to add
        @param: unique event index
        @return: true if event was added, false otherwise
    */
    bool addEvent(const LightDimmingEventAndCallback &new_event, const size_t event_index);

    /*
        @brief: remove event from light controller
        @param: event_index - index of event to remove
        @return: true if event was removed, false otherwise
    */
    bool removeEvent(const size_t event_index);

    /*
        @brief: update dimming time
        @param: new_dimming_time - new dimming time
        @param: event_index - index of event to update
        @return: true if dimming time was updated, false otherwise
    */
    bool updateEventDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const size_t event_index);

    /*
        @brief: update activation and deactivation time
        @param: new_activation_time - new activation time
        @param: new_deactivation_time - new deactivation time
        @return: true if activation and deactivation time was updated, false otherwise
    */
    bool updateActivationAndDeactivationTime(const ProjectTypes::time_minute_t &new_activation_time,
                                             const ProjectTypes::time_minute_t &new_deactivation_time,
                                             const size_t event_index);

    /*
        @brief: get rest of dimming time
        @param: current_time - current time
        @param: event_index - index of event
        @return: rest of dimming time in minutes
    */
    std::time_t getRestOfDimmingTime(const std::time_t &current_time, const size_t event_index) const;

    /*
        *   @brief: get rest of dimming time in percent
        *   @param: current_time - current time
        *   @param: event_index - index of event
        *   @return: rest of dimming time in percent [0.0 - 1.0]
    */
    float getRestOfDimmingTimePercent(const std::time_t &current_time, const size_t event_index) const;

    /*
        @brief: get total of dimming time
        @param: current_time - current time
        @return: total of dimming time in minutes
    */
    std::time_t getTotalOfDimmingTime(const std::time_t &current_time) const;

    /*
        @brief: get total of dimming time in percent
        @param: current_time - current time
        @return: total of dimming time in percent [0.0 - 1.0]
    */
    float getTotalOfDimmingTimePercent(const std::time_t &current_time) const;
    /*
        @brief: update all events
        @param: current_time - current time
    */
    void updateEvents(const std::time_t &current_time);

    /*
        @brief: update dimming time for all events
        @param: new_dimming_time - new dimming time
    */
    void updateAllDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time);

    /*
        @brief: update activation and deactivation time for all events
        @param: new_activation_time - new activation time
        @param: new_deactivation_time - new deactivation time
    */
    void updateAllActivationAndDeactivationTime(const ProjectTypes::time_minute_t &new_activation_time,
                                                         const ProjectTypes::time_minute_t &new_deactivation_time);

    /*
        @brief: get rest of dimming time for all events
        @param: current_time - current time
        @return: etl vector of rest of dimming time for each instance of light controller
    */
    RestDimmingTimeMap getAllRestOfDimmingTime(const std::time_t &current_time) const;

    /*
        @brief: get rest of dimming time for all events
        @param: current_time - current time
        @return: etl vector of rest percentage [0.0 - 1.0] of dimming time for each instance of light controller
    */
    RestDimmingTimePercentMap getAllRestOfDimmingTimePercent(const std::time_t &current_time) const;

 private:
    LightDimmingEventMap event_containers_;

    bool checkEventIndexIsValid(const size_t &event_index) const;

    RestDimmingTimeMap::iterator getActiveDimmingEvent(const std::time_t &current_time) const;

    size_t getActiveDimmingEventIndex(const std::time_t &current_time) const;

    std::time_t last_update_time_;
};  //  class LightEventController

}  //  namespace ControlLogic
