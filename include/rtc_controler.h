#ifndef RTC_CONTROLER_H
#define RTC_CONTROLER_H

#include <Arduino.h>
#include <DS1302.h>
#include "project_pin_definition.h"

constexpr unsigned int minYearWhenYearValueIsSmallerRtcIsUninitialized = 2020;
constexpr unsigned int maxYearWhenYearValueIsHigherRtcIsDisconnected = 2100;

enum class RtcSource
{
    Internal,
    External,
    None
};

enum class RtcStatus
{
    Ok,
    Uninitialized,
    Disconnected
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
void setRtcSource(const RtcSource rtc_source_to_set);

/*
    @details get rtc source
*/
RtcSource getRtcSource();

/*
    @details init rtc
*/
void initRtc(void);

/*
    @details get rtc status
    @return RtcStatus enum with current status of rtc
*/
RtcStatus getRtcStatus(void);

/*
    @details check input value is in range
    index 0 - year
    index 1 - month
    index 2 - day
    index 3 - hour
    index 4 - minute
    index 5 - second
    @return true if value is in range, false otherwise
*/
bool checkRtcValueIsInRange(uint32_t value, size_t index);

#endif // RTC_CONTROLER_H