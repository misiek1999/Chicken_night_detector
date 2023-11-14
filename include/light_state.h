#pragma once

namespace LightControl
{

// Enum class for light mode machine state
enum class LightState
{
    Off,        // Default state
    Blanking,   // Light is slowly blanking
    On,         // Light is on
    Error,      // Error state
    Undefined,  // Undefined state
    NumOfStates // Number of states
};

} // namespace LightControl
