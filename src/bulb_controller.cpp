#include "bulb_controller.h"
#include "calculate_sunset_and_sunrise_time.h"


// LightDimmingEvent class implementation
ControlLogic::LightDimmingEvent::LightDimmingEvent(const std::time_t &event_time,
                                     const ProjectTypes::time_minute_t &time_to_turn_on_before_event,
                                     const ProjectTypes::time_minute_t &time_to_turn_off_after_event,
                                     const ProjectTypes::time_minute_t &time_to_start_dimming_before_turn_on,
                                     const ProjectTypes::time_minute_t &time_to_end_dimming_after_turn_off):
        turn_on_event_(event_time, time_to_turn_on_before_event, time_to_turn_off_after_event),
        start_dimming_event_(event_time, time_to_start_dimming_before_turn_on + time_to_turn_on_before_event,
        time_to_end_dimming_after_turn_off + time_to_turn_off_after_event),
        time_to_turn_off_after_event_(time_to_turn_off_after_event),
        time_to_turn_on_before_event_(time_to_turn_on_before_event),
        time_to_start_dimming_before_turn_on_(time_to_start_dimming_before_turn_on),
        time_to_end_dimming_after_turn_off_(time_to_end_dimming_after_turn_off) {
}

ControlLogic::LightState ControlLogic::LightDimmingEvent::getLightState(const std::time_t &current_time) const {
    if (turn_on_event_.checkEventIsActive(current_time)) {
        return LightState::On;
    }
    if (start_dimming_event_.checkEventIsActive(current_time)) {
        return LightState::Dimming;
    }
    return LightState::Off;
}

void ControlLogic::LightDimmingEvent::setEventTime(const std::time_t & new_event_time) {
    turn_on_event_.setEventTime(new_event_time);
    start_dimming_event_.setEventTime(new_event_time);
}

void ControlLogic::LightDimmingEvent::setNewActivationAndDimmingTime(const ProjectTypes::time_minute_t & time_to_turn_on_before_event,
                                                                     const ProjectTypes::time_minute_t & time_to_turn_off_after_event,
                                                                     const ProjectTypes::time_minute_t & time_to_start_dimming_before_turn_on,
                                                                     const ProjectTypes::time_minute_t & time_to_end_dimming_after_turn_off) {
    time_to_turn_on_before_event_ = time_to_turn_on_before_event;
    time_to_turn_off_after_event_ = time_to_turn_off_after_event;
    time_to_start_dimming_before_turn_on_ = time_to_start_dimming_before_turn_on;
    time_to_end_dimming_after_turn_off_ = time_to_end_dimming_after_turn_off;
    updateEventActivationAndDimmingTime();
}

void ControlLogic::LightDimmingEvent::setNewDimmingTime(const ProjectTypes::time_minute_t & time_to_start_dimming_before_turn_on,
                                                        const ProjectTypes::time_minute_t & time_to_end_dimming_after_turn_off) {
    time_to_start_dimming_before_turn_on_ = time_to_start_dimming_before_turn_on;
    time_to_end_dimming_after_turn_off_ = time_to_end_dimming_after_turn_off;
    updateEventActivationAndDimmingTime();
}

void ControlLogic::LightDimmingEvent::setNewActivationTime(const ProjectTypes::time_minute_t & time_to_turn_on_before_event,
                                                           const ProjectTypes::time_minute_t & time_to_turn_off_after_event) {
    time_to_turn_on_before_event_ = time_to_turn_on_before_event;
    time_to_turn_off_after_event_ = time_to_turn_off_after_event;
    updateEventActivationAndDimmingTime();
}

ProjectTypes::time_minute_t ControlLogic::LightDimmingEvent::getRestLightDimmingTime(const std::time_t &current_time) const {
    auto rest_dimming_time = 0;
    auto light_state = getLightState(current_time);
    // check light state is in Dimming mode
    if (light_state == LightState::Dimming) {
        if (start_dimming_event_.checkEventWasHappened(current_time)) {
            rest_dimming_time = convertSecondsToMinutes(start_dimming_event_.getStopEventTime() - current_time);
        } else {
            rest_dimming_time = convertSecondsToMinutes(turn_on_event_.getEventTime() - current_time);
        }
        // TODO: add logs
    }
    return rest_dimming_time;
}

float ControlLogic::LightDimmingEvent::getRestLightDimmingTimePercent(const std::time_t &current_time) const {
    auto rest_dimming_time = getRestLightDimmingTime(current_time);
    auto current_dimming_time_ = getCurrentDimmingTime(current_time);
    const auto percent = static_cast<float>(rest_dimming_time) / static_cast<float>(current_dimming_time_);
    return percent;
}

void ControlLogic::LightDimmingEvent::updateEventActivationAndDimmingTime() {
    turn_on_event_.setNewTimeToTurnOffAfterEvent(time_to_turn_off_after_event_);
    turn_on_event_.setNewTimeToTurnOnBeforeEvent(time_to_turn_on_before_event_);
    start_dimming_event_.setNewTimeToTurnOffAfterEvent(time_to_end_dimming_after_turn_off_ + time_to_turn_off_after_event_);
    start_dimming_event_.setNewTimeToTurnOnBeforeEvent(time_to_start_dimming_before_turn_on_ + time_to_turn_on_before_event_);
}

ProjectTypes::time_minute_t ControlLogic::LightDimmingEvent::getCurrentDimmingTime(const std::time_t & current_time) const {
    if (start_dimming_event_.checkEventWasHappened(current_time)) {
        return time_to_end_dimming_after_turn_off_;
    }
    return time_to_start_dimming_before_turn_on_;
}


// -----------------------------------------------------------------------------
//                  LightBulbController class implementation
// -----------------------------------------------------------------------------

ControlLogic::LightBulbController::LightBulbController(const LightDimmingEventMap &events_containers,
                                                       const LightUpdateCallback &updateEventTimeCallback):
    event_containers_(events_containers),
    update_event_time_callback_(updateEventTimeCallback) {
}

ControlLogic::LightState ControlLogic::LightBulbController::getLightState(const std::time_t & current_time, const size_t event_index) {
    updateEvents(current_time);
    auto light_state = LightState::Error;
    if (checkEventIndexIsValid(event_index)) {
        light_state = event_containers_.at(event_index).getLightState(current_time);
    }
    return light_state;
}

ControlLogic::LightStateMap ControlLogic::LightBulbController::getAllLightState(const std::time_t & current_time) {
    updateEvents(current_time);
    LightStateMap light_state_map;
    for (auto event : event_containers_) {
        auto itr = event.first;
        light_state_map[itr] = getLightState(current_time, itr);
    }
    return light_state_map;
}

bool ControlLogic::LightBulbController::addEvent(const LightDimmingEvent &new_event, const size_t event_index) {
    bool result = false;
    if (event_containers_.full() == false) {
        if (!checkEventIndexIsValid(event_index)) {
            event_containers_.insert(etl::make_pair(event_index, new_event));
            result = true;
        } else {
            // TODO: add logs
        }
    } else {
        // TODO: add logs
    }
    return result;
}

bool ControlLogic::LightBulbController::removeEvent(const size_t event_index) {
    auto result = false;
    auto itr = event_containers_.find(event_index);
    if (itr != event_containers_.end()) {
        event_containers_.erase(itr);
        result = true;
    } else {
        //TODO: add logs
    }
    return result;
}

bool ControlLogic::LightBulbController::updateDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time, const size_t event_index) {
    bool status = false;
    if (checkEventIndexIsValid(event_index)) {
        event_containers_.at(event_index).setNewDimmingTime(new_dimming_time, new_dimming_time);
        status = true;
    }
    return status;
}

bool ControlLogic::LightBulbController::updateActivationAndDeactivationTime(const ProjectTypes::time_minute_t & new_activation_time,
                                                                            const ProjectTypes::time_minute_t & new_deactivation_time,
                                                                            const size_t event_index) {
    bool status = false;
    if (checkEventIndexIsValid(event_index)) {
        event_containers_.at(event_index).setNewActivationTime(new_activation_time, new_deactivation_time);
        status = true;
    }
    return status;
}

std::time_t ControlLogic::LightBulbController::getRestOfDimmingTime(const std::time_t & current_time, const size_t event_index) const {
    auto rest_dimming_time = 0;
    if (checkEventIndexIsValid(event_index)) {
        rest_dimming_time = event_containers_.at(event_index).getRestLightDimmingTime(current_time);
    }
    return rest_dimming_time;
}

float ControlLogic::LightBulbController::getRestOfDimmingTimePercent(const std::time_t & current_time, const size_t event_index) const {
    auto rest_dimming_time_percent = 0.0F;
    if (checkEventIndexIsValid(event_index)) {
        rest_dimming_time_percent = event_containers_.at(event_index).getRestLightDimmingTimePercent(current_time);
    }
    return rest_dimming_time_percent;
}

void ControlLogic::LightBulbController::updateEvents(const std::time_t & current_time) {
    // get new event time from callback
    if (update_event_time_callback_ == nullptr) {
        //  TODO: add error log
        return;
    }
    auto new_event_time = update_event_time_callback_(current_time);
    for (auto& event_container : event_containers_) {
        event_container.second.setEventTime(new_event_time);
    }
}

void ControlLogic::LightBulbController::setUpdateEventTimeCallback(const LightUpdateCallback & updateEventTimeCallback) {
    update_event_time_callback_ = updateEventTimeCallback;
}

void ControlLogic::LightBulbController::updateAllDimmingTime(const ProjectTypes::time_minute_t & new_dimming_time) {
    for (auto& event_container : event_containers_) {
        event_container.second.setNewDimmingTime(new_dimming_time, new_dimming_time);
    }
}

void ControlLogic::LightBulbController::updateAllActivationAndDeactivationTime(const ProjectTypes::time_minute_t & new_activation_time,
                                                                               const ProjectTypes::time_minute_t & new_deactivation_time) {
    for (auto& event_container : event_containers_) {
        event_container.second.setNewActivationTime(new_activation_time, new_deactivation_time);
    }
}

ControlLogic::RestDimmingTimeMap ControlLogic::LightBulbController::getAllRestOfDimmingTime(const std::time_t & current_time) const {
    RestDimmingTimeMap rest_dimming_time_map;
    for (auto event : event_containers_) {
        rest_dimming_time_map[event.first] = event.second.getRestLightDimmingTime(current_time);
    }
    return rest_dimming_time_map;
}

ControlLogic::RestDimmingTimePercentMap ControlLogic::LightBulbController::getAllRestOfDimmingTimePercent(const std::time_t & current_time) const {
    RestDimmingTimePercentMap rest_dimming_time_percent_map;
    for (auto event : event_containers_) {
        rest_dimming_time_percent_map[event.first] = event.second.getRestLightDimmingTimePercent(current_time);
    }
    return rest_dimming_time_percent_map;
}

bool ControlLogic::LightBulbController::checkEventIndexIsValid(const size_t & event_index) const {
    return event_containers_.find(event_index) != event_containers_.end();
}
