#pragma once

#include <Arduino.h>
#include "project_types.h"

namespace GPIO
{

class GpioDriver
{
public:
    GpioDriver() noexcept;

    void init();
    void toggleLight(const bool state);
    void setPWMLightPercentage(const float percent_light);
    void setPWMLight(const uint16_t pwm);
    bool getControlSelectSignal();
    ProjectTypes::analog_signal_t getLightAnalogSensorValue();
    ProjectTypes::analog_signal_t getControlAnalogSensorValue();

private:

};

extern GpioDriver gpio_driver;

}   // GPIO
