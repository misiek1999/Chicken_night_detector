#pragma once

#include <functional>

using lightStateCallback = std::function<bool()>;

enum class EnternalLightState {
    Low = 0,
    High
};
