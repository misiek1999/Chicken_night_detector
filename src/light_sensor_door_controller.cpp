#include "light_sensor_door_controller.h"
#include "Arduino.h"

constexpr const auto kDelayToChangeDoorStateMs = 300000;    // 5 minutes

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
        // This logic is inverted, if the light is high, the door should be closed
        door_state_ = current_light_state == EnternalLightState::High ? DoorControl::DoorControlAction::Close : DoorControl::DoorControlAction::Open;
    }
    return true;
}

DoorControl::DoorControlAction ControlLogic::LightSensorDoorController::getDoorState() const {
    return door_state_;
}