#include "light_control.h"
#include <functional>
#include "gpio_driver.h"
#include "project_const.h"
#include "rtc_driver.h"

ProjectTypes::abs_min_past_midnight_t ger_rtc_time_abs() {
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    return rtc_time.getAbsTimePostMidnight();
}

LightControl::LightController::LightController():
    daytime_calculator_(ProjectConst::kInstallationLatitude,
                                    ProjectConst::kInstallationLongitude,
                                    ProjectConst::kInstallationTimeZone,
                                    ProjectConst::kInstallationReq) {
    // Calculate current sunset and sunrise time to rtc light controller
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    ProjectTypes::abs_min_past_midnight_t event_time = daytime_calculator_.getSunsetTime(rtc_time);
    const ProjectTypes::time_minute_diff_t time_to_turn_on_before_event = ProjectConst::kLightControlSecondsToTurnOnLights/60;
    const ProjectTypes::time_minute_diff_t time_to_turn_off_after_event = ProjectConst::kLightControlSecondsToTurnOffLights/60;
    const ProjectTypes::time_minute_diff_t time_to_blink_before_event = 0;
    const ProjectTypes::time_minute_diff_t time_to_blink_after_event = ProjectConst::kLightControlSecondsToBlankingLight/60;

    light_controller_rtc_.addEvent((LightEventAndUpdateCallback)
                 {LightEvent(event_time,
                time_to_turn_on_before_event,
                time_to_turn_off_after_event,
                time_to_blink_before_event,
                time_to_blink_after_event),
             std::bind(&DaytimeCalculator::getSunsetTime,
                daytime_calculator_, std::placeholders::_1)});
}

bool LightControl::LightController::periodicUpdateLightState() {
    //TODO: rebuild this code
    bool result = true;
    // get current rtc time
    ProjectTypes::RTC_Time rtc_time;
    rtc_driver.getCurrentTimeRtc(rtc_time);
    // update selected light controller
    this->updateLightState(rtc_time);
    // read light state
    this->light_state_ = this->light_controller_rtc_.getLightState(rtc_time);
    // update light state
    switch (light_state_) {
    case LightState::On:
        turnOnLight();
        break;
    case LightState::Off:
        turnOffLight();
        break;
    case LightState::Blanking:
        lightBlanking(this->light_controller_rtc_.getRestOfBlankingTimePercent(rtc_time));
        break;
    case LightState::Undefined:
        Serial.println("LightControl::LightController::periodicUpdateLightState() - LightState::Undefined");
        turnOffLight();
        result = false;
        break;
    case LightState::Error:
    default:
        Serial.println("LightControl::LightController::periodicUpdateLightState() - LightState::Error");
        turnOffLight();
        result = false;
        break;
    }
    return result;
}

bool LightControl::LightController::changeBlankingTime(const ProjectTypes::abs_min_past_midnight_t & new_blanking_time,  const size_t &event_id)
{
    return light_controller_rtc_.updateBlankingTime(new_blanking_time, event_id);
}

LightControl::LightState LightControl::LightController::getLightState() const
{
    return light_state_;
}

bool LightControl::LightController::updateEvents(const ProjectTypes::RTC_Time & time_now)
{
    light_controller_rtc_.updateEvents(time_now);
    return true;
}

LightControl::LightState LightControl::LightController::updateLightState(const ProjectTypes::RTC_Time & time_now) {
    // Detect night using RTC
    light_state_ = light_controller_rtc_.getLightState(time_now);
    return light_state_;
}

void LightControl::LightController::turnOffLight() {
    GPIO::gpio_driver.toggleLight(false);
}

void LightControl::LightController::turnOnLight() {
    GPIO::gpio_driver.toggleLight(true);
}

void LightControl::LightController::lightBlanking(const float &percent_blanking) {
    GPIO::gpio_driver.setPWMLightPercentage(percent_blanking);
}
