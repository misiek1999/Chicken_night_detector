#pragma once

#include <Arduino.h>
#include <ctime>
#include <functional>
#include <etl/vector.h>
#include <etl/array.h>
#include "project_pin_definition.h"
#include "project_const.h"
#include "chicken_coop_config.h"
#include "calculate_sunset_and_sunrise_time.h"
#include "light_state.h"
#include "bulb_controller.h"
#include "gpio_driver.h"
#include "rtc_driver.h"
/*
    * @brief: Control logic namespace
    * @details: This namespace contains all classes and functions for control logic

*/
namespace ControlLogic {
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
        * @brief: Get light state
        * @return: map of current light states
    */
    ControlLogic::LightStateMap getLightStates() const;

    /*
        * @brief: Get light state
        * @param state: true - activate controller, false - disable controller
    */
    void toggleLightExternalBuilding(const bool &state);

    /*
        * @brief: Check if light controller is active in external building
        * @return: true if controller is active, false if controller is disabled
    */
    bool checkLightControllerInExternalBuildingIsActive() const;
    // TODO: Implement rest of interface requied to door control

 private:
    DaytimeCalculator daytime_calculator_;
    LightBulbController bulb_controller_;
    CoopConfig coop_config_;
    TimeCallback getRtcTime_;
    // TODO: add door controller
    ControlLogic::LightStateMap light_states_;

};

}  //  namespace ControlLogic
