#include "light_sensor_bulb_controller.h"
#include "log.h"
#include <Arduino.h>

ControlLogic::LightSensorBulbController::LightSensorBulbController(lightStateCallback light_sensor_callback):
        light_sensor_callback_(light_sensor_callback) {
}

bool ControlLogic::LightSensorBulbController::periodicUpdateController() {
    const auto current_time = millis();
    const auto current_light_state = light_sensor_callback_() ? LightState::On : LightState::Off;
    if (current_light_state != light_state_) {
        light_state_ = current_light_state;
        last_light_change_time_ = current_time;
    }
    if (current_time - last_light_change_time_ > kDelayToDisableLightMs) {
        light_state_ = LightState::Off;
    }
    LOG_DEBUG("Light state: %d", static_cast<int>(light_state_));
    return true;
}

ControlLogic::LightState ControlLogic::LightSensorBulbController::getLightState() {
    return light_state_;
}
