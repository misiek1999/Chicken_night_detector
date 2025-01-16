#include "chicken_coop_controller.h"
#include <functional>
#include "gpio_driver.h"
#include "project_const.h"
#include "rtc_driver.h"
#include "light_event_duration.h"
#include "log.h"
#include "light_sensor_controller instance.h"


ControlLogic::ChickenCoopController::ChickenCoopController(CoopConfig coop_config, TimeCallback get_rtc_time_callback):
        daytime_calculator_(ProjectConst::kInstallationLatitude,
                            ProjectConst::kInstallationLongitude,
                            ProjectConst::kInstallationTimeZone,
                            ProjectConst::kInstallationReq),
        coop_config_(coop_config),
        rtc_callback_(get_rtc_time_callback)
{
    // Add rtc controller to light controller interface map
    for (const auto &[callback, id , state] : coop_config_.light_config_) {
        rtc_bulb_controllers_.insert(etl::make_pair(id, RtcBulbController(&rtc_callback_, &daytime_calculator_)));
    }
    // Add light sensor bulb controller to light controller interface map
    for (const auto &[callback, id , state] : coop_config_.door_config_) {
        light_sensor_bulb_controllers_.insert(etl::make_pair(id, *getLightSensorBulbControllerInstance()));
    }

    // Add all RTC bulb controllers to main bulb controller map
    for (auto &[id, rtc_bulb_controller] : rtc_bulb_controllers_) {
        bulb_controllers_[id] = &rtc_bulb_controller;
    }

    // Add door controller event to main building
    auto getDoorOpenEventTime = [&] (const std::time_t &current_time) mutable {
        auto tm  = *std::localtime(&current_time);
        tm.tm_hour = 6;
        tm.tm_min = 0;
        auto new_open_time = std::mktime(&tm);
        return new_open_time;
    };
    auto getDoorCloseEventTime = [&] (const std::time_t &current_time) mutable {
        return daytime_calculator_.getSunsetTime(current_time) + ProjectConst::kLightControlSecondsToTurnOffLights;
    };
    const auto building_id = coop_config_.door_config_[0].id_;
    rtc_door_controllers_.insert(etl::make_pair(building_id,
                                RtcDoorController {
                                    DoorEventMap { etl::pair {
                                        getBuildingNumber(building_id),
                                        DoorEventAndCallback {
                                            TimeEvent(),
                                            DoorEventUpdateCallbacks {
                                                std::move(getDoorOpenEventTime),
                                                std::move(getDoorCloseEventTime)
                                            }
                                        }
                                    }},
                                    &rtc_callback_
                                }));
    // Add rtc controller to door controller interface map
    door_controllers_.insert(etl::make_pair(building_id, &rtc_door_controllers_.at(building_id)));

    // Add external light door controller
    light_sensor_door_controllers_.insert(etl::make_pair(building_id,
                                                         *getLightSensorDoorControllerInstance()));
}

bool ControlLogic::ChickenCoopController::periodicUpdateController() {
    bool result = true;
    LOG_VERBOSE("Chicken coop controller enter");
    // get current rtc time
    const std::time_t rtc_time = std::invoke(rtc_callback_);\
    LOG_VERBOSE("Current time: %d", rtc_time);
    // update door controller
    updateDoorController(rtc_time);
    // update light controller
    updateLightController(rtc_time);
    return result;
}

bool ControlLogic::ChickenCoopController::changeDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const BuildingId &building_id) {
    rtc_bulb_controllers_.at(building_id).updateAllDimmingTime(new_dimming_time);
    return true;
}

ProjectTypes::time_minute_t ControlLogic::ChickenCoopController::getDimmingTime(const BuildingId &building_id) const {
    const auto time = rtc_callback_();
    return rtc_bulb_controllers_.at(building_id).getTotalOfDimmingTime(time);
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

void ControlLogic::ChickenCoopController::setDoorControllerMode(const DoorControllerMode &mode) {
    LOG_INFO("Set door controller mode to %d", static_cast<int>(mode));
    door_controller_mode_ = mode;

    auto updateDoorController = [&](auto &controller_map, const char *controller_type) {
        for (auto &[buildingId, doorController] : door_controllers_) {
            if (controller_map.find(buildingId) == controller_map.end()) {
                LOG_WARNING("%s door controller for building %d does not exist", controller_type, buildingId);
                continue;
            }
            doorController = &controller_map.at(buildingId);
        }
    };

    switch (mode) {
        case DoorControllerMode::Rtc:
            updateDoorController(rtc_door_controllers_, "Rtc");
            break;
        case DoorControllerMode::ExternalLightSensor:
            updateDoorController(light_sensor_door_controllers_, "Light sensor");
            break;
        default:
            LOG_WARNING("Invalid door controller mode %d", static_cast<int>(mode));
            break;
    }
}

void ControlLogic::ChickenCoopController::setBulbControllerMode(const BulbControllerMode &mode) {
    if (mode >= BulbControllerMode::NumberOfBulbController) {
        LOG_WARNING("Invalid bulb controller mode %d", static_cast<int>(mode));
        return;
    }

    if (bulb_controller_mode_ == mode) {
        LOG_INFO("Bulb controller mode is already set to %d", static_cast<int>(mode));
        return;
    }

    LOG_INFO("Set bulb controller mode to %d", static_cast<int>(mode));
    bulb_controller_mode_ = mode;

    auto updateBulbController = [&](auto &controller_map, const char *controller_type) {
        for (auto &[buildingId, bulbController] : bulb_controllers_) {
            if (controller_map.find(buildingId) == controller_map.end()) {
                LOG_WARNING("%s bulb controller for building %d does not exist", controller_type, buildingId);
                continue;
            }
            bulbController = &controller_map.at(buildingId);
        }
    };

    switch (mode) {
        case BulbControllerMode::Rtc:
            updateBulbController(rtc_bulb_controllers_, "Rtc");
            break;
        case BulbControllerMode::ExternalLightSensor:
            updateBulbController(light_sensor_bulb_controllers_, "Light sensor");
            break;
        default:
            LOG_WARNING("Invalid bulb controller mode %d", static_cast<int>(mode));
            break;
    }
}

void ControlLogic::ChickenCoopController::updateDoorController(const std::time_t & rtc_time) {
    LOG_VERBOSE("Door controller enter");
    // Check all door controllers
    for (auto &[buildingId, doorController] : door_controllers_) {
        auto door_action = DoorControl::DoorControlAction::Disable;
        auto door_state_conf = coop_config_.door_config_.at(getBuildingNumber(buildingId));
        if (door_state_conf.is_active_) {
            LOG_VERBOSE("Door controller %d is active", buildingId);
            if (doorController->updateDoorControllerEvents()) {
                door_action = doorController->getDoorState();
                LOG_INFO("Door controller %d action: %d", buildingId, door_action);
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
    LOG_VERBOSE("Door controller updated exit");
}

void ControlLogic::ChickenCoopController::updateLightController(const std::time_t & rtc_time) {
    // iterate over all bulb light controllers
    for (auto &[buildingId, bulbController] : bulb_controllers_) {
        // update light controller
        std::ignore = bulbController->periodicUpdateController();
        // get current light state
        auto bulb_light_state = bulbController->getLightState();
        auto light_state_conf = coop_config_.light_config_.at(getBuildingNumber(buildingId));
        // When light controller is not active, force turn off the light
        if (!light_state_conf.is_active_) {
            bulb_light_state = LightState::Off;
        }
        light_states_[getBuildingNumber(buildingId)] = bulb_light_state;
        // set control signal to output control elements
        const auto dimming_prec = bulbController->getTotalOfDimmingTimePercent();
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
