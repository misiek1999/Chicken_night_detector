#pragma once

#include "door_control_states.h"
#include "time_event.h"
#include "rtc_driver.h"
#include <etl/unordered_map.h>
#include <etl/utility.h>
#include <ctime>


namespace ControlLogic {

constexpr size_t kMaxDoorEvent = 1;
using DoorEventUpdateCallbacks = etl::pair<EventUpdateCallback, EventUpdateCallback>;
using DoorEventAndCallback = etl::pair<TimeEvent, DoorEventUpdateCallbacks>;
using DoorEventMap = etl::unordered_map<size_t, DoorEventAndCallback, kMaxDoorEvent>;

bool checkCallbacksAreValid(const DoorEventUpdateCallbacks &callbacks);

/*
    * Interface to controll door
*/
class IDoorController {
    public:
        virtual ~IDoorController() = default;
        /*
            @brief: Update door controller, should be called periodically
            @return: true if success, false if failed
        */
        virtual bool updateDoorControllerEvents() = 0;
        /*
            @brief: Get door state
            @return: door action state
        */
        virtual DoorControl::DoorControlAction getDoorState() const = 0;
};

/*
    * Class to controll door based on rtc time
*/
class RtcDoorController : public IDoorController {
 public:
    RtcDoorController() = default;
    /*
        @brief: Constructor of class
        @param: door_event_map - map of door events
    */
    explicit RtcDoorController(const DoorEventMap& door_event_map, TimeCallback* rtc_callback);

    /*
        @brief: Update door controller, should be called periodically
        @return: true if success, false if failed
    */
    bool updateDoorControllerEvents() override;

    /*
        @brief: Get door state
        @return: door action state
    */
    DoorControl::DoorControlAction getDoorState() const override;

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
    DoorEventMap door_events_map_ = {};
    TimeCallback* rtc_callback_ = {};
};

}   // namespace ControlLogic
