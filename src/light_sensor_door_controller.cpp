#include "light_sensor_door_controller.h"
#include "Arduino.h"

constexpr const auto kDelayToChangeDoorStateMs = 1800000;    // 30 minutes

ControlLogic::LightSensorDoorController::LightSensorDoorController(lightStateCallback light_sensor_callback):
    light_sensor_callback_(light_sensor_callback) {
}

bool ControlLogic::LightSensorDoorController::updateDoorControllerEvents() {
    const auto current_time = millis();
    const auto current_light_state = light_sensor_callback_() ? EnternalLightState::High : EnternalLightState::Low;
    if (current_light_state != last_light_state_) {
        last_light_change_time_ = current_time;
        last_light_state_ = current_light_state;
    }
    if (current_time - last_light_change_time_ > kDelayToChangeDoorStateMs) {
        door_state_ = current_light_state == EnternalLightState::High ? DoorControl::DoorControlAction::Open : DoorControl::DoorControlAction::Close;
    }
    return true;
}

DoorControl::DoorControlAction ControlLogic::LightSensorDoorController::getDoorState() const {
    return door_state_;
}