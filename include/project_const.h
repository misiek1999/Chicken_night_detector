/*
    This file contains const values for project
*/
#ifndef  PROJECT_CONST_H
#define  PROJECT_CONST_H

#include <cinttypes>
#include "DS1302.h"
#include "project_types.h"

namespace ProjectConst
{
    // main program const
    constexpr ProjectTypes::time_ms_t kMainLoopCountToProcessLightDetect = 100;
    constexpr ProjectTypes::time_ms_t kMainLoopDelayUs = 10000; // 10 ms
    constexpr uint32_t kPhotoresistorSensorCount = 5;
    constexpr float kLightControlMinimumDutyCycle = 0.5f;
    constexpr int kMinutesToTurnOnLightBeforeSunset = 60;
    // light control const
    constexpr uint32_t kLightControlSecondsToTurnOffLights = 120 * 60; // Default value 2h = 120min
    constexpr uint32_t kLightControlSecondsToBlankingLight = 10 * 60;   // Default value 10min
    constexpr uint32_t kLightControlNightMaximumSecondDuration = 24 * 60 * 60;   // if control stuck in night mode, reset mcu after 24 hours
    constexpr uint32_t kLightControlSecondsBetweenChangeLightMode = 60;   // threshold for change light mode [s]
    // sunset and sunrise const
    constexpr float kInstallationLatitude = 50.08027485662493f;
    constexpr float kInstallationLongitude = 21.341718388675318f;
    constexpr int8_t kInstallationTimeZone = 1;
    constexpr float kInstallationReq = -0.833f;
} // namespace ProjectConst
#endif // PROJECT_CONST_H