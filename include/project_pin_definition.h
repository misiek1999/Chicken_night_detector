#pragma once
#include <Arduino.h>

// control outputs
constexpr auto kPinMainLigthOutput = PB0;
constexpr auto kPinExternalLigthOutput = PB1;
constexpr auto kPinMainDoorOutputPowerOn = PB2;
constexpr auto kPinMainDoorOutputUp = PB3;
constexpr auto kPinMainDoorOutputDown = PB4;

// control switches
constexpr auto kPinEnableMainLight = PB5;
constexpr auto kPinEnableExternalLight = PB6;
constexpr auto kPinEnableAutoDoorControl = PB7;
constexpr auto kPinEnableManualDoorControl = PB8;
constexpr auto kPinControlDoorOpen = PA0;
constexpr auto kPinControlDoorClose = PA1;

// control indicators
constexpr auto kPinOnboardLed = PC13;
constexpr auto kPinMainLightIndicator = PA2;
constexpr auto kPinExternalLightIndicator = PA3;
constexpr auto kPinErrorIndicator = PA4;

// DS1302 RTC module
constexpr auto kPinDS1302Clk = PA8;
constexpr auto kPinDS1302Data = PA9;
constexpr auto kPinDS1302En = PA10;
