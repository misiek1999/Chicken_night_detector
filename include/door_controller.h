#pragma once

#include <Arduino.h>
#include <ctime>
#include <etl/utility.h>
#include <etl/unordered_map.h>
#include "door_control_states.h"
#include "time_event.h"


namespace ControlLogic {

constexpr size_t kMaxDoorEvent = 1;
using DoorEventUpdateCallbacks = etl::pair<EventUpdateCallback, EventUpdateCallback>;
using DoorEventAndCallback = etl::pair<TimeEvent, DoorEventUpdateCallbacks>;
using DoorEventMap = etl::unordered_map<size_t, DoorEventAndCallback, kMaxDoorEvent>;

bool checkCallbacksAreValid(const DoorEventUpdateCallbacks &callbacks);
/*
    * Class to controll door
*/
class DoorController {
 public:
    DoorController() = default;
    /*
        @brief: Constructor of class
        @param: door_event_map - map of door events
    */
    explicit DoorController(DoorEventMap door_event_map);

    /*
        @brief: Update door controller, should be called periodically
        @param: current_time - current time
        @return: true if success, false if failed
    */
    bool updateDoorControllerEvents(const std::time_t &current_time);

    /*
        @brief: Get door state
        @return: door action state
    */
    DoorControl::DoorControlAction getDoorState(const std::time_t &current_time);

    /*
        @brief: Add door event
        @param: door_event_map - new door events map
        @param: return: true if success, false if failed
    */
    bool addDoorEvent(const DoorEventMap &new_events_map);

    /*
        @brief: Remove door event
        @param: door_event_id - id of door event to remove
        @return: true if success, false if failed
    */
    bool removeDoorEvent(const size_t &door_event_id);

 private:
    DoorEventMap door_events_map_;
};

}   // namespace ControlLogic
