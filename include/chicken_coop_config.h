#pragma once

#include <Arduino.h>
#include "door_control_states.h"

namespace ControlLogic {

constexpr auto kMaxLightBulb = 2u;
constexpr auto kMaxDoor = 1u;

/*
    * @brief: Building id
    * @details: Enum class for building id
    *          Main - Main building
    *          External - External building, active only during summer time
*/
enum class BuildingId {
    Main = 0,       // Main building
    External,       // External building, active only during summer time
    NumberOfBuildings
};

/*
    * @brief: Door controller mode used by coop controller
    * @details: Enum class for door controller mode
    *          Rtc - Door controller based on RTC time
    *          ExternalLightSensor - Door controller based on external light sensor
    *          NumberOfDoorController - Number of door controllers
*/
enum class DoorControllerMode {
    Rtc = 0,
    ExternalLightSensor,
    NumberOfDoorController
};

enum class BulbControllerMode {
    Rtc = 0,
    ExternalLightSensor,
    NumberOfBulbController
};

struct LightStateGpioCallback {
    std::function<void(const bool &)> toogle_light_state;
    std::function<void(const float &)> set_pwm_light_percentage;
};

struct DoorOpenCloseGpioCallback {
    std::function<void(const DoorControl::DoorControlAction &)> toogle_door_state;
};

struct LightStateConfig {
    LightStateGpioCallback callback_;
    BuildingId id_;             // building id
    bool is_active_ = true;     // default is active
};

struct DoorStateConfig {
    DoorOpenCloseGpioCallback callback_;
    BuildingId id_;             // building id
    bool is_active_ = true;     // default is active
};

using LightBulbConfigArray = etl::array<LightStateConfig, kMaxLightBulb>;
using DoorConfigArray = etl::array<DoorStateConfig, kMaxDoor>;

struct CoopConfig {
    LightBulbConfigArray light_config_;
    DoorConfigArray door_config_;
};

constexpr size_t getBuildingNumber(const BuildingId &building_id) {
    return static_cast<size_t>(building_id);
}

}  // namespace ControlLogic
