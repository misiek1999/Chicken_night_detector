/*
    This file include function to control light
*/
#pragma once

#include <Arduino.h>
#include <etl/vector.h>
#include "project_pin_definition.h"
#include "project_const.h"
#include "light_state.h"
#include "light_controller_rtc.h"
#include "gpio_driver.h"
#include "calculate_sunset_and_sunrise_time.h"
#include "light_sensor_driver.h"

namespace LightControl
{

enum class LightControlMode
{
    RTC,            // Recommenced mode
    LightSensor,    // Unrecommended mode in dark place
    None,
    NumOfLightControlModes
};

class LightController
{
public:
    LightController();

    bool periodicUpdateLightState();
    bool changeBlankingTime(const ProjectTypes::abs_min_past_midnight_t &new_blanking_time, const size_t &event_id);
    void changeLightControlMode(const LightControlMode &new_mode);

private:
    LightSensorDriver light_sensor_driver_;
    LightControllerRTC light_controller_rtc_;
    DaytimeCalculator daytime_calculator_;
    LightControlMode light_control_mode_;
    LightState light_state_;

    // Update events in light control
    bool updateEvents(const ProjectTypes::RTC_Time &time_now);
    LightState updateLightState(const ProjectTypes::RTC_Time &time_now);

    // Light control methods
    void turnOffLight();
    void turnOnLight();
    void lightBlanking(const float &percent_blanking);
};

} // namespace LightControl
