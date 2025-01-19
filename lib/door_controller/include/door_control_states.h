#pragma once

#define CASE_ENUM_TO_STRING(enum_name) \
    case enum_name: return #enum_name;

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

inline const char* getDoorControlModeName(const DoorControlMode& mode) {
    switch (mode)
    {
    CASE_ENUM_TO_STRING(DoorControlMode::Off);
    CASE_ENUM_TO_STRING(DoorControlMode::Auto);
    CASE_ENUM_TO_STRING(DoorControlMode::Manual);
    CASE_ENUM_TO_STRING(DoorControlMode::Error);
    CASE_ENUM_TO_STRING(DoorControlMode::NumberOfModes);
    default:
        break;
    }
    return "Unknown";
}

inline const char* getDoorControlActionName(const DoorControlAction& action) {
    switch (action)
    {
    CASE_ENUM_TO_STRING(DoorControlAction::Disable);
    CASE_ENUM_TO_STRING(DoorControlAction::Open);
    CASE_ENUM_TO_STRING(DoorControlAction::Close);
    CASE_ENUM_TO_STRING(DoorControlAction::NumberOfSignals);
    default:
        break;
    }
    return "Unknown";
}
}   // namespace DoorControl

#undef CASE_ENUM_TO_STRING
