#include "chicken_coop_controller.h"
#include <functional>
#include "gpio_driver.h"
#include "project_const.h"
#include "rtc_driver.h"
#include "light_event_duration.h"
#include "log.h"

EventUpdateCallback sunset_callback;

ControlLogic::ChickenCoopController::ChickenCoopController(CoopConfig coop_config, TimeCallback rtc_callback):
        daytime_calculator_(ProjectConst::kInstallationLatitude,
                            ProjectConst::kInstallationLongitude,
                            ProjectConst::kInstallationTimeZone,
                            ProjectConst::kInstallationReq),
        bulb_controllers_(),
        door_controllers_(),
        coop_config_(coop_config),
        rtc_callback_(rtc_callback),
        light_states_(),
        door_actions_(),
        last_change_time_(0),
        last_door_action_(DoorControl::DoorControlAction::Disable) {
    // check provided pointers to rtc and gpio drivers are not null
    assert(rtc_callback_ != nullptr && "RTC time callback is nullptr!");
    // Calculate current sunset and sunrise time to rtc light controller
    std::time_t rtc_time = rtc_callback_();
    auto event_time = daytime_calculator_.getSunsetTime(rtc_time);
    const auto date = *std::localtime(&rtc_time);
    const auto month = static_cast<uint8_t>(date.tm_mon);
    const auto [time_to_blink_before_event, time_to_blink_after_event, time_to_turn_on_before_event, time_to_turn_off_after_event] = ControlLogic::getEventDurationTime(month);

    // Add bulb light controller callback to update sunrise and sunset time
    sunset_callback = std::bind(&DaytimeCalculator::getSunsetTime,
                &daytime_calculator_, std::placeholders::_1);

    // Add main building to bulb controller
    auto building_id = coop_config_.light_config_[0].id_;
    bulb_controllers_.insert(etl::make_pair(building_id,
                                LightBulbController(
                                    LightDimmingEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        LightDimmingEventAndCallback {
                                            .event =
                                                LightDimmingEvent(event_time,
                                                    0,
                                                    time_to_turn_off_after_event,
                                                    time_to_blink_before_event,
                                                    time_to_blink_after_event),
                                            .callback = sunset_callback
                                        }
                                    }}
    )));

    // Add external building to bulb controller
    building_id = coop_config_.light_config_[1].id_;
    bulb_controllers_.insert(etl::make_pair(building_id,
                                LightBulbController {
                                    LightDimmingEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        LightDimmingEventAndCallback {
                                            .event =
                                                LightDimmingEvent(event_time,
                                                    0,
                                                    time_to_turn_off_after_event,
                                                    time_to_blink_before_event,
                                                    time_to_blink_after_event),
                                            .callback = sunset_callback
                                        }
                                    }}
                                }));
    // Add door controller event to main building
    auto doorEventFirstUpdate = [&] (const std::time_t &current_time) mutable {
        auto tm  = *std::localtime(&current_time);
        tm.tm_hour = 6;
        tm.tm_min = 0;
        auto new_open_time = std::mktime(&tm);
        return new_open_time;
    };
    auto doorEventSecondUpdate = [&] (const std::time_t &current_time) mutable {
        return sunset_callback(current_time) + ProjectConst::kLightControlSecondsToTurnOffLights;
    };
    building_id = coop_config_.door_config_[0].id_;
    door_controllers_.insert(etl::make_pair(building_id,
                                RtcDoorController {
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
    const std::time_t rtc_time = std::invoke(rtc_callback_);
    // update door controller
    updateDoorController(rtc_time);
    // update light controller
    updateLightController(rtc_time);
    return result;
}

bool ControlLogic::ChickenCoopController::changeDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const BuildingId &building_id) {
    bulb_controllers_.at(building_id).updateAllDimmingTime(new_dimming_time);
    return true;
}

ProjectTypes::time_minute_t ControlLogic::ChickenCoopController::getDimmingTime(const BuildingId &building_id) const {
    const auto time = rtc_callback_();
    return bulb_controllers_.at(building_id).getTotalOfDimmingTime(time);
}

ControlLogic::LightStateMap ControlLogic::ChickenCoopController::getLightStates() const {
    return light_states_;
}

void ControlLogic::ChickenCoopController::toggleLightExternalBuilding(const bool & state) {
    const auto building_id = getBuildingNumber(BuildingId::External);
    coop_config_.light_config_[building_id].is_active_ = state;
    LOG_INFO("Toggle external building light to %s", state ? "on" : "off");
}

void ControlLogic::ChickenCoopController::toggleLightMainBuilding(const bool & state) {
    const auto building_id = getBuildingNumber(BuildingId::Main);
    coop_config_.light_config_[building_id].is_active_ = state;
    LOG_INFO("Toggle main building light to %s", state ? "on" : "off");
}

bool ControlLogic::ChickenCoopController::checkLightControllerInExternalBuildingIsActive() const {
    const auto building_id = getBuildingNumber(BuildingId::External);
    const auto state = coop_config_.light_config_[building_id].is_active_;
    LOG_DEBUG("External building light is %s", state ? "on" : "off");
    return state;
}

bool ControlLogic::ChickenCoopController::checkLightControllerInMainBuildingIsActive() const {
    const auto building_id = getBuildingNumber(BuildingId::Main);
    const auto state = coop_config_.light_config_[building_id].is_active_;
    LOG_DEBUG("Main building light is %s", state ? "on" : "off");
    return state;
}

ControlLogic::DoorActionMap ControlLogic::ChickenCoopController::getDoorActions() const {
    return door_actions_;
}

void ControlLogic::ChickenCoopController::toggleAutomaticDoorController(const bool &state) {
    const auto building_id = getBuildingNumber(BuildingId::Main);
    coop_config_.door_config_[building_id].is_active_ = state;
    LOG_INFO("Toggle automatic door controller to %s", state ? "on" : "off");
}

void ControlLogic::ChickenCoopController::updateDoorController(const std::time_t & rtc_time) {
    // Check all door controllers
    for (auto &[buildingId, doorController] : door_controllers_) {
        auto door_action = DoorControl::DoorControlAction::Disable;
        auto door_state_conf = coop_config_.door_config_.at(getBuildingNumber(buildingId));
        if (door_state_conf.is_active_) {
            if (doorController.updateDoorControllerEvents(rtc_time)) {
                door_action = doorController.getDoorState(rtc_time);
            }
            // if door state has changed, update last change time and last door action
            if (door_action != last_door_action_) {
                last_change_time_ = rtc_time;
                last_door_action_ = door_action;
                LOG_INFO("Door controller %d action changed to %d", buildingId, door_action);
            }
            // After kMaxDoorMovementTime seconds of door movement, disable door controller to save power
            if (door_action == DoorControl::DoorControlAction::Open ||
                door_action == DoorControl::DoorControlAction::Close) {
                // if door is moving too long, disable door controller
                if (std::abs(std::difftime(rtc_time, last_change_time_)) > kMaxDoorMovementTime) {
                    door_action = DoorControl::DoorControlAction::Disable;
                }
            }
            door_state_conf.callback_.toogle_door_state(door_action);
            door_actions_[getBuildingNumber(buildingId)] = door_action;
        }
    }
}

void ControlLogic::ChickenCoopController::updateLightController(const std::time_t & rtc_time) {
    // iterate over all bulb light controllers
    for (auto &[buildingId, lightBulbController] : bulb_controllers_) {
        // update dimming and light event duration time
        const auto month = (*std::localtime(&rtc_time)).tm_mon;
        const auto [time_to_blink_before_event, time_to_blink_after_event, time_to_turn_on_before_event, time_to_turn_off_after_event] = ControlLogic::getEventDurationTime(month);
        lightBulbController.updateEventDimmingTime(time_to_blink_after_event, getBuildingNumber(buildingId));
        lightBulbController.updateActivationAndDeactivationTime(time_to_turn_on_before_event, time_to_turn_off_after_event, getBuildingNumber(buildingId));
        LOG_VERBOSE("Update dimming time %d for %d", time_to_blink_after_event, getBuildingNumber(buildingId));
        LOG_VERBOSE("Update activation time Bef: %d Aft: %d for %d", time_to_turn_on_before_event, time_to_turn_off_after_event, getBuildingNumber(buildingId));
        // get current light state
        auto bulb_light_state = lightBulbController.getLightState(rtc_time);
        auto light_state_conf = coop_config_.light_config_.at(getBuildingNumber(buildingId));
        // When light controller is not active, force turn off the light
        if (!light_state_conf.is_active_) {
            bulb_light_state = LightState::Off;
        }
        light_states_[getBuildingNumber(buildingId)] = bulb_light_state;
        // set control signal to output control elements
        auto dimming_prec = lightBulbController.getTotalOfDimmingTimePercent(rtc_time);
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
}
