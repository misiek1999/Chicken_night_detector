#pragma once

#include <Arduino.h>
#include "door_control_states.h"

namespace ControlLogic {

constexpr auto kMaxLightBulb = 2;
constexpr auto kMaxDoor = 1;

/*
    * @brief: Building id
    * @details: Enum class for building id
    *          Main - Main building
    *          External - External building, active only during summer time
*/
enum class BuildingId {
    Main,       // Main building
    External,   // External building, active only during summer time
    NumberOfBuildings
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
    bool is_active_;            // default is active
};

struct DoorStateConfig {
    DoorOpenCloseGpioCallback callback_;
    BuildingId id_;             // building id
    bool is_active_ = true;     // default is active
};

using LightBulbConfigArray = etl::array<LightStateConfig, kMaxLightBulb>;
using DoorConfigArray = etl::array<DoorStateConfig, kMaxDoor>;

struct CoopConfig {
    LightBulbConfigArray light_state_config_;
    DoorConfigArray door_config_;
};

inline size_t getBuildingNumber(const BuildingId &building_id) {
    return static_cast<size_t>(building_id);
}

using TimeCallback = std::function<std::time_t(void)>;
}  // namespace ControlLogic
