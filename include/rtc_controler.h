#ifndef RTC_CONTROLER_H
#define RTC_CONTROLER_H

#include <Arduino.h>
#include <DS1302.h>
#include "project_pin_definition.h"

enum class RtcSource
{
    Internal,
    External,
    None
};

/*
    @details get current time from rtc
*/
void getCurrentTimeRtc(Time &current_time);

/*
    @details set current time to rtc
*/
void setTimeToRtc(Time &time_to_set);

/*
    @details set rtc source
*/
void setRtcSource(RtcSource rtc_source_to_set);

/*
    @details init rtc
*/
void initRtc(void);

#endif // RTC_CONTROLER_H