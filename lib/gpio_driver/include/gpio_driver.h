#pragma once

#include <Arduino.h>
#include <door_control_states.h>

namespace GPIOInterface {

class GpioDriver {
 private:
    GpioDriver();

 public:
    /*
        * Get instance of GpioDriver
        * @brief This is a singleton class
        * @return instance of GpioDriver
    */
    static GpioDriver *getInstance() noexcept {
        static GpioDriver gpio_driver;
        return &gpio_driver;
    }

    /*
        * Toggle light in main building
        * @param state - state to set
    */
    void toggleLightMainBuilding(const bool state);

    /*
        * Set PWM light percentage in main building
        * @param percent_light - percentage of light to set
    */
    void setPWMLightPercentageMainBuilding(const float percent_light);

    /*
        * Toggle light in external building
        * @param state - state to set
    */
    void toggleLightExternalBuilding(const bool state);

    /*
        * Set PWM light percentage in external building
        * @param percent_light - percentage of light to set
    */
    void setPWMLightPercentageExternalBuilding(const float percent_light);

    /*
        * Set light state to selected pin
        * @param state - state to set
        * @param pin - pin number to set
    */
    void setNormalLightState(const bool state, const uint16_t pin);

    /*
        * Set PWM light
        * @param percent_light - number of PWM to set [0 - 1.0]
        * @param pin - pin number to set
    */
    void setPWMLight(const float percent_light, const uint16_t pin);

    /*
        * Check that light in main building should enabled
        * @return true if control select signal is high, false otherwise
    */
    bool getMainBuildingEnableControl();

    /*
        * Check that light in external building should enabled
        * @return true if control select signal is high, false otherwise
    */
    bool getExternalBuildingEnableControl();

    /*
        * Check selected mode of door control in main building
        * @return state of door control select signal
    */
    DoorControl::DoorControlMode getMainBuildingDoorControlMode();

    /*
        * Check door open signal in main building is active
        * @return true if door open signal is active, false otherwise
    */
    bool checkDoorControlOpenSignalIsActive();

    /*
        * Check door close signal in main building is active
        * @return true if door close signal is active, false otherwise
    */
    bool checkDoorControlCloseSignalIsActive();

    /*
        * Set door control action
        * @brief Set door control action
        *       Disable - dont perform any action
        *       Open - open door
        *       Close - close door
        * @param action - action to set
    */
    void setDoorControlAction(const DoorControl::DoorControlAction action);

    /*
        * Toogle power save mode
        @param state - true if power mode should be enabled, otherwise disable
    */
    void togglePowerSaveMode(const bool state);

    /*
        * Set error indicator
    */
    void setErrorIndicator();

    /*
        * Clear error indicator
    */
    void clearErrorIndicator();

    /*
        * Toggle error indicator
        * @param state - true if error indicator should be enabled, otherwise disable
    */
    void toggleErrorIndicator(const bool state);

    /*
        @details get external light sensor value
        @return true if external light sensor is active, false otherwise
    */
    bool getExternalLightSensor();

 private:
    /*
        When power save mode is enabled and doors are moving, then controller disable light bulbs to decrease power consumption
    */
    bool power_save_mode_;
    /*
        True if one of door is opening
    */
    bool doors_are_opening_;

    void toggleLightMainBuildingIndicator(const bool state);

    void toggleLightExternalBuildingIndicator(const bool state);

    void toggleDoorMoveIndicator(const bool state);
};

}  //  namespace GPIOInterface
