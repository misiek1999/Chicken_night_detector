#pragma once

#include <Arduino.h>
#include <ctime>
#include <functional>
#include <etl/vector.h>
#include <etl/array.h>
#include <etl/unordered_map.h>
#include "project_pin_definition.h"
#include "project_const.h"
#include "chicken_coop_config.h"
#include "calculate_sunset_and_sunrise_time.h"
#include "light_state.h"
#include "light_event_manager.h"
#include "door_controller.h"
#include "gpio_driver.h"
#include "rtc_driver.h"
#include "light_sensor_controller.h"
/*
    * @brief: Control logic namespace
    * @details: This namespace contains all classes and functions for control logic

*/
namespace ControlLogic {
constexpr uint8_t kMaxLightController = 2;
using LightBulbControllerMap = etl::unordered_map<BuildingId, LightEventManager, kMaxLightController>;
using RtcDoorControllerMap = etl::unordered_map<BuildingId, RtcDoorController, kMaxLightController>;
using LightSensorDoorControllerMap = etl::unordered_map<BuildingId, LightSensorDoorController, kMaxLightController>;
using DoorControllerMap = etl::unordered_map<BuildingId, IDoorController*, kMaxLightController>;
using DoorActionMap = etl::unordered_map<size_t, DoorControl::DoorControlAction, kMaxEventsCount>;
/*
    * @brief: Main controller class for chicken coop
    * @details: This class is responsible for controlling the lights in the chicken coop and opening and closing the door
    *           Should be updated in the periodicUpdateController() function every 100ms
    *           The light controller is based on the RTC time
*/
class ChickenCoopController {
 public:
    ChickenCoopController() = delete;
    /*
        @brief: contructor of class
    */
    ChickenCoopController(CoopConfig coop_config, TimeCallback get_rtc_time);

    /*
        * @bierf: Update light controller
        * @details: This function should be called every 100ms
        * @return true if success, false if failed
    */
    bool periodicUpdateController();

    /*
        * @brief: Change dimming time
        * @param new_dimming_time: new dimming time in minutes
        * @param building_id: building id
        * @return: true if success, false if failed
    */
    bool changeDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const BuildingId &building_id);

    /*
        * @brief: Get dimming time
        * @param building_id: building id
        * @return: rest of dimming time
    */
    ProjectTypes::time_minute_t getDimmingTime(const BuildingId &building_id) const;

    /*
        * @brief: Get light state
        * @return: map of current light states
    */
    ControlLogic::LightStateMap getLightStates() const;

    /*
        * @brief: Toggle light controller in external building
        * @param state: true - activate controller, false - disable controller
    */
    void toggleLightExternalBuilding(const bool &state);

    /*
        * @brief: Toggle light controller in main building
        * @param state: true - activate controller, false - disable controller
    */
    void toggleLightMainBuilding(const bool &state);

    /*
        * @brief: Check if light controller is active in external building
        * @return: true if controller is active, false if controller is disabled
    */
    bool checkLightControllerInExternalBuildingIsActive() const;

    /*
        * @brief: Check if light controller is active in main building
        * @return: true if controller is active, false if controller is disabled
    */
    bool checkLightControllerInMainBuildingIsActive() const;

    /*
        * @brief: Get door action state
        * @return: DoorActionMap
    */
    DoorActionMap getDoorActions() const;

    /*
        * @brief: Toggle automatic door controller
        * @param state: true - enable automatic door controller, false - disable automatic door controller
    */
    void toggleAutomaticDoorController(const bool &state);

    /*
        * @brief: Get door controller mode
        * @return: DoorControllerMode
    */
    DoorControllerMode getDoorControllerMode() const {
        return door_controller_mode_;
    }

    /*
        * @brief: Set door controller mode
        * @param mode: DoorControllerMode
    */
    void setDoorControllerMode(const DoorControllerMode &mode);

 private:
    DaytimeCalculator daytime_calculator_;
    LightBulbControllerMap bulb_controllers_;
    DoorControllerMode door_controller_mode_;
    RtcDoorControllerMap rtc_door_controllers_;
    LightSensorDoorControllerMap light_sensor_door_controllers_;
    DoorControllerMap door_controllers_;
    CoopConfig coop_config_;
    TimeCallback rtc_callback_;
    ControlLogic::LightStateMap light_states_;
    DoorActionMap door_actions_;

    static constexpr double kMaxDoorMovementTime = 10.0;    // 10 seconds
    std::time_t last_change_time_;
    DoorControl::DoorControlAction last_door_action_;

    /*
        * @brief: Update door controller
        * @param rtc_time: RTC time
    */
    void updateDoorController(const std::time_t &rtc_time);

    /*
        * @brief: Update light controller
        * @param rtc_time: RTC time
    */
    void updateLightController(const std::time_t &rtc_time);
};

}  //  namespace ControlLogic
