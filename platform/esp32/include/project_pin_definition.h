#pragma once
#include <Arduino.h>

// control outputs
constexpr auto kPinMainLigthOutput = 13;
constexpr auto kPinExternalLigthOutput = 12;
constexpr auto kPinMainDoorOutputPowerOn = 14;
constexpr auto kPinMainDoorOutputUp = 27;
constexpr auto kPinMainDoorOutputDown = 26;

// control switches
constexpr auto kPinEnableMainLight = 15;
constexpr auto kPinEnableExternalLight = 2;
constexpr auto kPinEnableAutoDoorControl = 4;
constexpr auto kPinEnableManualDoorControl = 16;
constexpr auto kPinControlDoorOpen = 17;
constexpr auto kPinControlDoorClose = 5;

// control indicators
constexpr auto kPinOnboardLed = 36;
constexpr auto kPinMainLightIndicator = 39;
constexpr auto kPinExternalLightIndicator = 34;
constexpr auto kPinErrorIndicator = 35;
constexpr auto kPinDoorMoveIndicator = 32;

// DS3231 RTC module
constexpr auto kPinDS3231Clk = 22;
constexpr auto kPinDS3231Data = 21;
