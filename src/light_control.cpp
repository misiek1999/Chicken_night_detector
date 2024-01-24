#include "light_control.h"
#include <functional>
#include "gpio_driver.h"
#include "project_const.h"
#include "rtc_driver.h"

ProjectTypes::abs_min_past_midnight_t ger_rtc_time_abs()
{
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    return rtc_time.getAbsTimePostMidnight();
}

LightControl::LightController::LightController():
    daytime_calculator_(ProjectConst::kInstallationLatitude,
                                    ProjectConst::kInstallationLongitude,
                                    ProjectConst::kInstallationTimeZone,
                                    ProjectConst::kInstallationReq),
    light_control_mode_(LightControlMode::RTC)
{
    // Calculate current sunset and sunrise time to rtc light controller
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    ProjectTypes::abs_min_past_midnight_t event_time = daytime_calculator_.getSunsetTime(rtc_time);
    light_controller_rtc_.addEvent((LightEventAndUpdateCallback)
                 {LightEvent(event_time,
                ProjectConst::kLightControlSecondsToTurnOnLights,
                ProjectConst::kLightControlSecondsToTurnOffLights,
                ProjectConst::kLightControlSecondsToBlankingLight,
                ProjectConst::kLightControlSecondsToBlankingLight),
             std::bind(&DaytimeCalculator::getSunsetTime,
                daytime_calculator_, std::placeholders::_1)});
}

bool LightControl::LightController::periodicUpdateLightState()
{
    //TODO: rebuild this code 
    bool status = true;
    // get current rtc time
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    // update selected light controller
    this->light_controller_rtc_.updateEvents(rtc_time);
    // read light state
    

    return status;
}

bool LightControl::LightController::changeBlankingTime(const ProjectTypes::abs_min_past_midnight_t & new_blanking_time,  const size_t &event_id)
{
    return light_controller_rtc_.updateBlankingTime(new_blanking_time, event_id);
}

void LightControl::LightController::changeLightControlMode(const LightControlMode & new_mode)
{
    if (new_mode < LightControlMode::NumOfLightControlModes) {
        light_control_mode_ = new_mode;
    }
}

bool LightControl::LightController::updateEvents(const ProjectTypes::RTC_Time & time_now)
{
    // TODO: add support to outdoor light detection method
    light_controller_rtc_.updateEvents(time_now);

    return true;
}

LightControl::LightState LightControl::LightController::updateLightState(const ProjectTypes::RTC_Time & time_now)
{
    // Select mode of detection dark
    switch (light_control_mode_)
    {
    case LightControl::LightControlMode::RTC:          // Detect night using RTC
        light_state_ = light_controller_rtc_.getLightState(time_now);
        break;
    case LightControl::LightControlMode::LightSensor:  // Detect night using outdoor light sensor
        light_state_ = light_sensor_driver_.getLightState(time_now);
        break;
    case LightControl::LightControlMode::None:         // Do nothing
        light_state_ = LightState::Off;
        break;
    default:
        light_state_ = LightState::Error;
        break;
    }
    return light_state_;
}

void LightControl::LightController::turnOffLight()
{
    GPIO::gpio_driver.toggleLight(false);
}

void LightControl::LightController::turnOnLight()
{
    GPIO::gpio_driver.toggleLight(false);
}

void LightControl::LightController::lightBlanking(const float &percent_blanking)
{
    GPIO::gpio_driver.setPWMLightPercentage(percent_blanking);
}
