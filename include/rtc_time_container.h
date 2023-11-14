#pragma once
#include <cstdint>
#include "project_types.h"

// Copy of class from library DS1302.h
class RtcTimeContainer {
public:
    enum Day {
        kSunday    = 1,
        kMonday    = 2,
        kTuesday   = 3,
        kWednesday = 4,
        kThursday  = 5,
        kFriday    = 6,
        kSaturday  = 7
    };

    // Creates a Time object with a given time.
    //
    // Args:
    //   yr: year. Range: {2000, ..., 2099}.
    //   mon: month. Range: {1, ..., 12}.
    //   date: date (of the month). Range: {1, ..., 31}.
    //   hr: hour. Range: {0, ..., 23}.
    //   min: minutes. Range: {0, ..., 59}.
    //   sec: seconds. Range: {0, ..., 59}.
    //   day: day of the week. Sunday is 1. Range: {1, ..., 7}.
    RtcTimeContainer(): sec(0), min(0), hr(0), date(0), mon(0), day((Day)0), yr(0) {};
    RtcTimeContainer(uint16_t yr, uint8_t mon, uint8_t date,
        uint8_t hr, uint8_t min, uint8_t sec,
        Day day): sec(sec), min(min), hr(hr), date(date), mon(mon), day(day), yr(yr) {};

    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t date;
    uint8_t mon;
    Day day;
    uint16_t yr;

    ProjectTypes::abs_min_past_midnight_t getAbsTimePostMidnight() const {
        return (ProjectTypes::abs_min_past_midnight_t)(min + hr * 60);
    }
};

namespace ProjectTypes
{
    using RTC_Time = RtcTimeContainer;
}
