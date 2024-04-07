/*
    This file contains const values for project
*/
#ifndef  PROJECT_CONST_H
#define  PROJECT_CONST_H

#include <cinttypes>
#include "project_types.h"

namespace ProjectConst
{
    // main program const
    constexpr unsigned kSerialBaudRate = 115200U;
    constexpr ProjectTypes::time_ms_t kMainLoopDelayBetweenLightControlProcess = 1000;  // 1000ms
    constexpr ProjectTypes::time_us_t kMainLoopDelayUs = 10000; // 10 ms
    constexpr uint32_t kPhotoresistorSensorCount = 5;
    constexpr float kLightControlMinimumDutyCycle = 0.5F;

    // light control const
    constexpr uint32_t kLightControlSecondsToTurnOnLights = 60 * 60; // Default value 1h
    constexpr uint32_t kLightControlSecondsToTurnOffLights = 60 * 60; // Default value 1h
    constexpr uint32_t kLightControlSecondsToDimmingLight = 10 * 60;   // Default value 10min
    constexpr uint32_t kLightControlNightMaximumSecondDuration = 24 * 60 * 60;   //Maximum duration of ON or Dimming mode in light control logic
    // sunset and sunrise const
    constexpr ProjectTypes::latitude_t kInstallationLatitude = 50.08027485662493F;
    constexpr ProjectTypes::longitude_t kInstallationLongitude = 21.341718388675318F;
    constexpr ProjectTypes::time_zone_t kInstallationTimeZone = 1;
    constexpr ProjectTypes::req_t kInstallationReq = -0.833F;
    // external time module const
    constexpr uint8_t kMonthSyncOffset = 1;
} // namespace ProjectConst
#endif // PROJECT_CONST_H
