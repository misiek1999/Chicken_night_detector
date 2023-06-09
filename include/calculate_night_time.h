#ifndef CALCULATE_NIGHT_TIME_H
#define CALCULATE_NIGHT_TIME_H

#include <Arduino.h>
#include <sunset.h>
#include "rtc_controler.h"
#include "project_const.h"

/*
    @details function to detect night using RTC
*/
bool detectNightUsingRtc(void);

#endif // CALCULATE_NIGHT_TIME_H