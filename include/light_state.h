#pragma once

namespace ControlLogic {

// Enum class for light mode machine state
enum class LightState {
    Off,         // Default state
    Dimming,     // Light is slowly dimming
    On,          // Light is on
    Error,       // Error state
    Undefined,   // Undefined state
    NumOfStates  // Number of states
};

}  //  namespace ControlLogic
