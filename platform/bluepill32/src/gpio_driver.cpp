#include "gpio_driver.h"
#include <chrono>
#include "Wire.h"
#include "project_types.h"
#include "project_pin_definition.h"

constexpr uint32_t kChangeStateDelayUs = 5;
constexpr uint16_t kPwmFrequency = 500;
constexpr uint16_t kPwmMaxValue = 255;
constexpr uint16_t kPwmMinValue = 0;
constexpr float kLightPercentOff = 0.5;
constexpr float kLightPercentOn = 1.0;

GPIOInterface::GpioDriver::GpioDriver():
        power_save_mode_(true),
        doors_are_opening_(false) {
    // initialize all gpio
    pinMode(kPinOnboardLed, OUTPUT);

    pinMode(kPinMainLigthOutput, OUTPUT);
    pinMode(kPinExternalLigthOutput, OUTPUT);

    pinMode(kPinMainDoorOutputDown, OUTPUT);
    pinMode(kPinMainDoorOutputUp, OUTPUT);
    pinMode(kPinMainDoorOutputPowerOn, OUTPUT);

    pinMode(kPinDoorMoveIndicator, OUTPUT);
    pinMode(kPinMainLightIndicator, OUTPUT);
    pinMode(kPinExternalLightIndicator, OUTPUT);
    pinMode(kPinErrorIndicator, OUTPUT);

    pinMode(kPinEnableMainLight, INPUT_PULLUP);
    pinMode(kPinEnableExternalLight, INPUT_PULLUP);
    pinMode(kPinEnableAutoDoorControl, INPUT_PULLUP);
    pinMode(kPinEnableManualDoorControl, INPUT_PULLUP);

    pinMode(kPinControlDoorClose, INPUT_PULLUP);
    pinMode(kPinControlDoorOpen, INPUT_PULLUP);

    analogWriteFrequency(kPwmFrequency);

    // lights are off by default
    digitalWrite(kPinOnboardLed, HIGH);
    digitalWrite(kPinMainLigthOutput, LOW);
    digitalWrite(kPinExternalLigthOutput, LOW);

    digitalWrite(kPinMainDoorOutputDown, LOW);
    digitalWrite(kPinMainDoorOutputUp, LOW);
    digitalWrite(kPinMainDoorOutputPowerOn, LOW);

    digitalWrite(kPinDoorMoveIndicator, LOW);
    digitalWrite(kPinMainLightIndicator, LOW);
    digitalWrite(kPinExternalLightIndicator, LOW);
    digitalWrite(kPinErrorIndicator, LOW);

    // Init i2c bus
    Wire.begin();
}

void GPIOInterface::GpioDriver::toggleLightMainBuilding(const bool state) {
    setNormalLightState(state, kPinMainLigthOutput);
    digitalWrite(kPinOnboardLed, static_cast<uint32_t>(!state));
}

void GPIOInterface::GpioDriver::setPWMLightPercentageMainBuilding(const float percent_light) {
    setPWMLight(percent_light, kPinMainLigthOutput);
    digitalWrite(kPinOnboardLed, HIGH);
}

void GPIOInterface::GpioDriver::toggleLightExternalBuilding(const bool state) {
    setNormalLightState(state, kPinExternalLigthOutput);
}

void GPIOInterface::GpioDriver::setPWMLightPercentageExternalBuilding(const float percent_light) {
    setPWMLight(percent_light, kPinExternalLigthOutput);
}

void GPIOInterface::GpioDriver::setNormalLightState(const bool state, const uint16_t pin) {
    if (state && !doors_are_opening_) {
        analogWrite(pin, kPwmMaxValue);
    } else {
        analogWrite(pin, kPwmMinValue);
    }
}

void GPIOInterface::GpioDriver::setPWMLight(const float percent_light, const uint16_t pin) {
    // scale percent range
    const float scaled_percent_light = percent_light * (kLightPercentOn - kLightPercentOff) + kLightPercentOff;
    // convert percent light to the range of 0 to 1023
    const auto pwm = static_cast<uint16_t>(scaled_percent_light * 255.0);
    analogWrite(pin, pwm);
}

bool GPIOInterface::GpioDriver::getMainBuildingEnableControl() {
    return digitalRead(kPinEnableMainLight);
}

bool GPIOInterface::GpioDriver::getExternalBuildingEnableControl() {
    return digitalRead(kPinEnableExternalLight);
}

DoorControl::DoorControlMode GPIOInterface::GpioDriver::getMainBuildingDoorControlMode() {
    auto state = DoorControl::DoorControlMode::Off;
    if (digitalRead(kPinEnableAutoDoorControl) == LOW) {
        state = DoorControl::DoorControlMode::Auto;
    } else if (digitalRead(kPinEnableManualDoorControl) == LOW) {
        state = DoorControl::DoorControlMode::Manual;
    }
    return state;
}

bool GPIOInterface::GpioDriver::checkDoorControlOpenSignalIsActive() {
    return digitalRead(kPinControlDoorOpen);
}

bool GPIOInterface::GpioDriver::checkDoorControlCloseSignalIsActive() {
    return digitalRead(kPinControlDoorClose);
}

void GPIOInterface::GpioDriver::setDoorControlAction(const DoorControl::DoorControlAction action) {
    doors_are_opening_ = power_save_mode_;  // by default set this member to true, if this option is enabled
    if (action == DoorControl::DoorControlAction::Open) {
        digitalWrite(kPinMainDoorOutputUp, HIGH);
        digitalWrite(kPinMainDoorOutputDown, LOW);
        delayMicroseconds(kChangeStateDelayUs);
        digitalWrite(kPinMainDoorOutputPowerOn, HIGH);
    } else if (action == DoorControl::DoorControlAction::Close) {
        digitalWrite(kPinMainDoorOutputUp, LOW);
        digitalWrite(kPinMainDoorOutputDown, HIGH);
        delayMicroseconds(kChangeStateDelayUs);
        digitalWrite(kPinMainDoorOutputPowerOn, HIGH);
    } else {
        digitalWrite(kPinMainDoorOutputPowerOn, LOW);
        delayMicroseconds(kChangeStateDelayUs);
        digitalWrite(kPinMainDoorOutputUp, LOW);
        digitalWrite(kPinMainDoorOutputDown, LOW);
        doors_are_opening_ = false;
    }
}

void GPIOInterface::GpioDriver::togglePowerSaveMode(const bool state) {
    power_save_mode_ = state;
}

void GPIOInterface::GpioDriver::setErrorIndicator() {
    toggleErrorIndicator(true);
}

void GPIOInterface::GpioDriver::clearErrorIndicator() {
    toggleErrorIndicator(false);
}

void GPIOInterface::GpioDriver::toggleLightMainBuildingIndicator(const bool state) {
    digitalWrite(kPinMainLightIndicator, state);
}

void GPIOInterface::GpioDriver::toggleLightExternalBuildingIndicator(const bool state) {
    digitalWrite(kPinExternalLightIndicator, state);
}

void GPIOInterface::GpioDriver::toggleDoorMoveIndicator(const bool state) {
    digitalWrite(kPinDoorMoveIndicator, state);
}

void GPIOInterface::GpioDriver::toggleErrorIndicator(const bool state) {
    digitalWrite(kPinErrorIndicator, state);
}
