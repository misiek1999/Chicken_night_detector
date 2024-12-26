#pragma once
#include <Arduino.h>

// actuators outputs
constexpr auto kPinMainLigthOutput = PB0;
constexpr auto kPinExternalLigthOutput = PB1;
constexpr auto kPinMainDoorOutputUp = PB3;
constexpr auto kPinMainDoorOutputDown = PB4;

// control switches
constexpr auto kPinEnableMainLight = PB5;
constexpr auto kPinEnableExternalLight = PB9;
constexpr auto kPinEnableAutoDoorControl = PA9;
constexpr auto kPinEnableManualDoorControl = PB8;
constexpr auto kPinControlDoorOpen = PA0;
constexpr auto kPinControlDoorClose = PA1;

// control indicators
constexpr auto kPinOnboardLed = PC13;
constexpr auto kPinMainLightIndicator = PA2;
constexpr auto kPinExternalLightIndicator = PA3;
constexpr auto kPinErrorIndicator = PA4;
constexpr auto kPinDoorMoveIndicator = PA5;


// DS3231 RTC module
constexpr auto kPinDS3231Clk = PB6;
constexpr auto kPinDS3231Data = PB7;
