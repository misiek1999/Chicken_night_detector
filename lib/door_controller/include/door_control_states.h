#pragma once

#include <Arduino.h>

namespace DoorControl {
/*
    * @brief Enum class for door control mode
    * @details Enum class for door control mode
*/
enum class DoorControlMode {
    Off,
    Auto,
    Manual,
    Error,
    NumberOfModes
};

/*
    * @brief Enum class for door control action
    * @details Enum class for door control action
*/
enum class DoorControlAction {
    Disable,
    Open,
    Close,
    NumberOfSignals
};
}   // namespace DoorControl
