#include "chicken_coop_controller.h"
#include <functional>
#include "gpio_driver.h"
#include "project_const.h"
#include "rtc_driver.h"


ControlLogic::ChickenCoopController::ChickenCoopController(CoopConfig coop_config, TimeCallback get_rtc_time):
    daytime_calculator_(ProjectConst::kInstallationLatitude,
                                ProjectConst::kInstallationLongitude,
                                ProjectConst::kInstallationTimeZone,
                                ProjectConst::kInstallationReq),
                                coop_config_(coop_config),
                                getRtcTime_(get_rtc_time) {
    // check provided pointers to rtc and gpio drivers are not null
    assert(gpio_driver_ != nullptr && getRtcTime_ != nullptr && "GPIO or getRtcTime is nullptr!");
    // Calculate current sunset and sunrise time to rtc light controller
    std::time_t rtc_time = getRtcTime_();
    auto event_time = daytime_calculator_.getSunsetTime(rtc_time);
    const ProjectTypes::time_minute_t time_to_turn_on_before_event = ProjectConst::kLightControlSecondsToTurnOnLights/60;
    const ProjectTypes::time_minute_t time_to_turn_off_after_event = ProjectConst::kLightControlSecondsToTurnOffLights/60;
    const ProjectTypes::time_minute_t time_to_blink_before_event = 0;
    const ProjectTypes::time_minute_t time_to_blink_after_event = ProjectConst::kLightControlSecondsToDimmingLight/60;

    // Add bulb light controller callback to update sunset time
    bulb_controller_.setUpdateEventTimeCallback(std::bind(&DaytimeCalculator::getSunsetTime,
                daytime_calculator_, std::placeholders::_1));
    // Add main building to bulb controller
    bulb_controller_.addEvent((LightDimmingEvent)
                                    {LightDimmingEvent(event_time,
                                    time_to_turn_on_before_event,
                                    time_to_turn_off_after_event,
                                    time_to_blink_before_event,
                                    time_to_blink_after_event)},
                                    getBuildingId(coop_config_.light_state_config_[0].id_));
    // Add external building to bulb controller
    bulb_controller_.addEvent((LightDimmingEvent)
                                    {LightDimmingEvent(event_time,
                                    time_to_turn_on_before_event,
                                    time_to_turn_off_after_event,
                                    time_to_blink_before_event,
                                    time_to_blink_after_event)},
                                    getBuildingId(coop_config_.light_state_config_[1].id_));
}

bool ControlLogic::ChickenCoopController::periodicUpdateController() {
    bool result = true;
    // get current rtc time
    std::time_t rtc_time = std::invoke(getRtcTime_);
    // update door controller
    // TODO: implement door controller

    // get all light states
    light_states_ = bulb_controller_.getAllLightState(rtc_time);
    auto dimming_times = bulb_controller_.getAllRestOfDimmingTimePercent(rtc_time);
    // perform actions on bulbs based on light states
    for (const auto &light_state_conf : coop_config_.light_state_config_) {
        auto bulb_id = getBuildingId(light_state_conf.id_);
        // find element in coop config
        auto light_state = light_states_.find(bulb_id);
        if (light_state == light_states_.end()) {
            // TODO: log error
            result = false;
            break;
        }
        // check if light controller is active
        if (light_state_conf.is_active_) {
            switch (light_state->second) {
            case LightState::On:
                light_state_conf.callback_.toogle_light_state(true);
                break;
            case LightState::Off:
                light_state_conf.callback_.toogle_light_state(false);
                break;
            case LightState::Dimming:
                light_state_conf.callback_.set_pwm_light_percentage(dimming_times[bulb_id]);
                break;
            default:
                break;
            }

        } else {
            light_state_conf.callback_.toogle_light_state(false);
        }
    }
    return result;
}

bool ControlLogic::ChickenCoopController::changeDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const BuildingId &building_id) {
    return bulb_controller_.updateDimmingTime(new_dimming_time, static_cast<size_t>(building_id));
}

ControlLogic::LightStateMap ControlLogic::ChickenCoopController::getLightStates() const {
    return light_states_;
}
