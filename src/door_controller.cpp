#include "door_controller.h"
#include "log.h"

ControlLogic::RtcDoorController::RtcDoorController(const DoorEventMap& door_event_map, const TimeCallback &rtc_callback):
    door_events_map_(door_event_map),
    rtc_callback_(rtc_callback) {
}


bool ControlLogic::RtcDoorController::updateDoorControllerEvents() {
    const auto current_time = rtc_callback_();
    for (auto &[door_id, door_event_and_callback] : door_events_map_) {
        const auto& door_callback = door_event_and_callback.second;
        if (checkCallbacksAreValid(door_callback)) {
            auto new_event_start = door_callback.first(current_time);
            auto new_event_stop = door_callback.second(current_time);
            door_event_and_callback.first.setEventTime(new_event_start, new_event_stop);
        } else {
            LOG_ERROR("Invalid callback for door event: %d", door_id);
        }
    }
    return true;
}

DoorControl::DoorControlAction ControlLogic::RtcDoorController::getDoorState() const {
    const auto current_time = rtc_callback_();
    auto action = DoorControl::DoorControlAction::Close;
    for (const auto &[door_id, door_event_and_callback] : door_events_map_) {
        if (door_event_and_callback.first.checkEventIsActive(current_time)) {
            action = DoorControl::DoorControlAction::Open;
            break;
        }
    }
    return action;
}

bool ControlLogic::RtcDoorController::addDoorEvent(const DoorEventMap & new_events_map) {
    auto result = true;
    for (auto &new_event : new_events_map) {
        if (door_events_map_.size() >= kMaxDoorEvent) {
            LOG_ERROR("Max door event reached");
            result = false;
            break;
        }
        if (door_events_map_.find(new_event.first) == door_events_map_.end()) {
            if (checkCallbacksAreValid(new_event.second.second)) {
                door_events_map_.insert(new_event);
                LOG_INFO("Door event added: %d", new_event.first);
            } else {
                LOG_ERROR("Invalid callback for door event: %d", new_event.first);
            }
        } else {
            LOG_WARNING("Door event already exist: %d", new_event.first);
        }
    }
    return result;
}

bool ControlLogic::RtcDoorController::removeDoorEvent(const size_t & door_event_id) {
    auto result = false;
    auto itr = door_events_map_.find(door_event_id);
    if (itr != door_events_map_.end()) {
        door_events_map_.erase(itr);
        result = true;
        LOG_INFO("Door event removed: %d", door_event_id);
    } else {
        LOG_WARNING("Door event not found: %d", door_event_id);
    }
    return result;
}

bool ControlLogic::checkCallbacksAreValid(const DoorEventUpdateCallbacks & callbacks) {
    if (callbacks.first == nullptr || callbacks.second == nullptr) {
        LOG_WARNING("Invalid callback");
        return false;
    }
    return true;
}
