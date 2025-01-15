#pragma once

#include <Functional>

using lightStateCallback = std::function<bool()>;

enum class EnternalLightState {
    Low = 0,
    High
};
