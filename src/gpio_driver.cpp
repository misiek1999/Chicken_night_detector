#include "gpio_driver.h"
#include <chrono>
#include "project_types.h"
#include "project_pin_definition.h"

constexpr uint32_t kChangeStateDelayUs = 5;
constexpr uint16_t kPwmFrequency = 500;
constexpr uint16_t kPwmMaxValue = 255;
constexpr uint16_t kPwmMinValue = 0;
constexpr float kLightPercentOff = 0.5;
constexpr float kLightPercentOn = 1.0;

GPIO::GpioDriver::GpioDriver() {
    // initialize all gpio
    pinMode(kPinOnboardLed, OUTPUT);
    pinMode(kPinMainLigthOutput, OUTPUT);
    analogWriteFrequency(kPwmFrequency);
    // lights are off by default
    digitalWrite(kPinMainLigthOutput, LOW);
    digitalWrite(kPinOnboardLed, HIGH);
}

void GPIO::GpioDriver::toggleLightMainBuilding(const bool state) {
    setNormalLightState(state, kPinMainLigthOutput);
    digitalWrite(kPinOnboardLed, static_cast<uint32_t>(state));
}

void GPIO::GpioDriver::setPWMLightPercentageMainBuilding(const float percent_light) {
    setPWMLight(percent_light, kPinMainLigthOutput);
    digitalWrite(kPinOnboardLed, HIGH);
}

void GPIO::GpioDriver::toggleLightExternalBuilding(const bool state) {
    setNormalLightState(state, kPinMainLigthOutput);
    digitalWrite(kPinOnboardLed, static_cast<uint32_t>(state));
}

void GPIO::GpioDriver::setPWMLightPercentageExternalBuilding(const float percent_light) {
    setPWMLight(percent_light, kPinExternalLigthOutput);
}

void GPIO::GpioDriver::setNormalLightState(const bool state, const uint16_t pin) {
    if (state) {
        analogWrite(pin, kPwmMaxValue);
    } else {
        analogWrite(pin, kPwmMinValue);
    }
}

void GPIO::GpioDriver::setPWMLight(const float percent_light, const uint16_t pin) {
    // scale percent range
    const float scaled_percent_light = percent_light * (kLightPercentOn - kLightPercentOff) + kLightPercentOff;
    // convert percent light to the range of 0 to 1023
    const auto pwm = static_cast<uint16_t>(scaled_percent_light * 255.0);
    analogWrite(pin, pwm);
}

bool GPIO::GpioDriver::getMainBuildingEnableControl() {
    return digitalRead(kPinEnableMainLight);
}

bool GPIO::GpioDriver::getExternalBuildingEnableControl() {
    return digitalRead(kPinEnableExternalLight);
}

DoorControl::DoorControlMode GPIO::GpioDriver::getMainBuildingDoorControlMode() {
    auto state = DoorControl::DoorControlMode::Off;
    if (digitalRead(kPinEnableAutoDoorControl) == LOW) {
        state = DoorControl::DoorControlMode::Auto;
    } else if (digitalRead(kPinEnableManualDoorControl) == LOW) {
        state = DoorControl::DoorControlMode::Manual;
    }
    return state;
}

bool GPIO::GpioDriver::checkDoorOpenSignalIsActive() {
    return digitalRead(kPinControlDoorOpen);
}

bool GPIO::GpioDriver::checkDoorCloseSignalIsActive() {
    return digitalRead(kPinControlDoorClose);
}

void GPIO::GpioDriver::setDoorControlAction(const DoorControl::DoorControlAction action) {
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
    }
}
