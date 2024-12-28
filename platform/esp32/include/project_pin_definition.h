#pragma once
#include <Arduino.h>

// actuators outputs
constexpr auto kPinMainLigthOutput = 16;
constexpr auto kPinExternalLigthOutput = 4;
constexpr auto kPinMainDoorOutputUp = 17;
constexpr auto kPinMainDoorOutputDown = 5;

// control switches
constexpr auto kPinEnableMainLight = 14;
constexpr auto kPinEnableExternalLight = 27;
constexpr auto kPinEnableAutoDoorControl = 26;
constexpr auto kPinEnableManualDoorControl = 25;
constexpr auto kPinControlDoorOpen = 33;
constexpr auto kPinControlDoorClose = 32;

// control indicators
constexpr auto kPinOnboardLed = 2;
constexpr auto kPinMainLightIndicator = 19;
constexpr auto kPinExternalLightIndicator = 18;
constexpr auto kPinErrorIndicator = 13;
constexpr auto kPinDoorMoveIndicator = 23;

// DS3231 RTC module
constexpr auto kPinDS3231Clk = 22;
constexpr auto kPinDS3231Data = 21;

// external light sensor
constexpr auto kPinExternalLightSensor = 34;
