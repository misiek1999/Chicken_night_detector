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
        bulb_controllers_(),
        door_controllers_(),
        coop_config_(coop_config),
        getRtcTime_(get_rtc_time),
        light_states_(),
        door_actions_() {
    // check provided pointers to rtc and gpio drivers are not null
    assert(gpio_driver_ != nullptr && getRtcTime_ != nullptr && "GPIO or getRtcTime is nullptr!");
    // Calculate current sunset and sunrise time to rtc light controller
    std::time_t rtc_time = getRtcTime_();
    auto event_time = daytime_calculator_.getSunsetTime(rtc_time);
    const ProjectTypes::time_minute_t time_to_turn_on_before_event = ProjectConst::kLightControlSecondsToTurnOnLights/60;
    const ProjectTypes::time_minute_t time_to_turn_off_after_event = ProjectConst::kLightControlSecondsToTurnOffLights/60;
    const ProjectTypes::time_minute_t time_to_blink_before_event = 0;
    const ProjectTypes::time_minute_t time_to_blink_after_event = ProjectConst::kLightControlSecondsToDimmingLight/60;

    // Add bulb light controller callback to update sunrise and sunset time
    auto sunset_callback = std::bind(&DaytimeCalculator::getSunsetTime,
                daytime_calculator_, std::placeholders::_1);
    auto sunrise_callback = std::bind(&DaytimeCalculator::getSunriseTime,
                daytime_calculator_, std::placeholders::_1);
    // Add main building to bulb controller
    auto building_id = coop_config_.light_state_config_[0].id_;
    bulb_controllers_.insert(etl::make_pair(building_id,
                                LightBulbController {
                                    LightDimmingEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        LightDimmingEventAndCallback {
                                            .event =
                                                LightDimmingEvent(event_time,
                                                    time_to_turn_on_before_event,
                                                    time_to_turn_off_after_event,
                                                    time_to_blink_before_event,
                                                    time_to_blink_after_event),
                                            .callback = sunset_callback
                                        }
                                    }}
                                }));

    // Add external building to bulb controller
    building_id = coop_config_.light_state_config_[1].id_;
    bulb_controllers_.insert(etl::make_pair(building_id,
                                LightBulbController {
                                    LightDimmingEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        LightDimmingEventAndCallback {
                                            .event =
                                                LightDimmingEvent(event_time,
                                                    time_to_turn_on_before_event,
                                                    time_to_turn_off_after_event,
                                                    time_to_blink_before_event,
                                                    time_to_blink_after_event),
                                            .callback = sunset_callback
                                        }
                                    }}
                                }));
    // Add door controller event to main building
    auto doorEventFirstUpdate = [&](const std::time_t &current_time) {
        auto tm  = *std::localtime(&current_time);
        tm.tm_hour = 6;
        tm.tm_min = 0;
        auto new_open_time = std::mktime(&tm);
        return sunrise_callback(new_open_time);
    };
    auto doorEventSecondUpdate = [&](const std::time_t &current_time) {
        return sunset_callback(current_time);
    };
    building_id = coop_config_.door_config_[0].id_;
    door_controllers_.insert(etl::make_pair(building_id,
                                DoorController {
                                    DoorEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        DoorEventAndCallback {
                                            TimeEvent(),
                                            DoorEventUpdateCallbacks {
                                                doorEventFirstUpdate,
                                                doorEventSecondUpdate
                                            }
                                        }
                                    }}
                                }));
}

bool ControlLogic::ChickenCoopController::periodicUpdateController() {
    bool result = true;
    // get current rtc time
    std::time_t rtc_time = std::invoke(getRtcTime_);
    // update door controller
    for (auto &door_controller : door_controllers_) {
        auto door_action = DoorControl::DoorControlAction::Disable;
        if (coop_config_.door_config_.at(getBuildingNumber(door_controller.first)).is_active_) {
            if (door_controller.second.updateDoorControllerEvents(rtc_time)) {
                door_action = door_controller.second.getDoorState(rtc_time);
            }
        }
        door_actions_[getBuildingNumber(door_controller.first)] = door_action;
    }
    // iterate over all bulb light controllers
    for (auto &light_controller : bulb_controllers_) {
        auto bulb_light_state = light_controller.second.getLightState(rtc_time);
        auto light_state_conf = coop_config_.light_state_config_.at(getBuildingNumber(light_controller.first));
        // When light controller is not active, force turn off the light
        if (!light_state_conf.is_active_) {
            bulb_light_state = LightState::Off;
        }
        light_states_[getBuildingNumber(light_controller.first)] = bulb_light_state;
        auto dimming_prec = light_controller.second.getTotalOfDimmingTimePercent(rtc_time);
        switch (bulb_light_state) {
            case LightState::On:
                light_state_conf.callback_.toogle_light_state(true);
                break;
            case LightState::Off:
                light_state_conf.callback_.toogle_light_state(false);
                break;
            case LightState::Dimming:
                light_state_conf.callback_.set_pwm_light_percentage(dimming_prec);
                break;
            default:
                break;
            }
    }
    return result;
}

bool ControlLogic::ChickenCoopController::changeDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const BuildingId &building_id) {
    bulb_controllers_.at(building_id).updateAllDimmingTime(new_dimming_time);
    return true;
}

ControlLogic::LightStateMap ControlLogic::ChickenCoopController::getLightStates() const {
    return light_states_;
}

ControlLogic::DoorActionMap ControlLogic::ChickenCoopController::getDoorActions() {
    return door_actions_;
}
