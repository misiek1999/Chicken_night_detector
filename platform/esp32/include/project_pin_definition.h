#pragma once
#include <Arduino.h>

// actuators outputs
constexpr auto kPinMainLigthOutput = 0;
constexpr auto kPinExternalLigthOutput = 2;
constexpr auto kPinMainDoorOutputPowerOn = 4;
constexpr auto kPinMainDoorOutputUp = 16;
constexpr auto kPinMainDoorOutputDown = 17;

// control switches
constexpr auto kPinEnableMainLight = 36;
constexpr auto kPinEnableExternalLight = 39;
constexpr auto kPinEnableAutoDoorControl = 34;
constexpr auto kPinEnableManualDoorControl = 35;
constexpr auto kPinControlDoorOpen = 32;
constexpr auto kPinControlDoorClose = 33;

// control indicators
constexpr auto kPinOnboardLed = 23;
constexpr auto kPinMainLightIndicator = 1;
constexpr auto kPinExternalLightIndicator = 3;
constexpr auto kPinErrorIndicator = 19;
constexpr auto kPinDoorMoveIndicator = 18;

// DS3231 RTC module
constexpr auto kPinDS3231Clk = 22;
constexpr auto kPinDS3231Data = 21;
