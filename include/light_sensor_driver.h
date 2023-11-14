#pragma once

#include <Arduino.h>
#include "project_types.h"
#include "light_state.h"
#include "rtc_time_container.h"

class LightSensorDriver
{
public:
    LightSensorDriver();

    /*
        Get light state from sensor
    */
    LightControl::LightState getLightState(const ProjectTypes::RTC_Time &time_now);

private:
    ProjectTypes::analog_signal_t sensor_value_;

    ProjectTypes::analog_signal_t readAnalogSignalFromSensor();

};
