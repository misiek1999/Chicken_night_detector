#pragma once
#include <cstdint>
#include "project_types.h"

// Constants for RTC container
constexpr unsigned int minYearWhenYearValueIsSmallerRtcIsUninitialized = 2020;
constexpr unsigned int maxYearWhenYearValueIsHigherRtcIsInvalid = 2100;
constexpr unsigned int minMonth = 1;
constexpr unsigned int maxMonth = 12;
constexpr unsigned int minDate = 1;
constexpr unsigned int maxDate = 31;
constexpr unsigned int minHour = 0;
constexpr unsigned int maxHour = 23;
constexpr unsigned int minMinute = 0;
constexpr unsigned int maxMinute = 59;
constexpr unsigned int minSecond = 0;
constexpr unsigned int maxSecond = 59;
constexpr unsigned int minDay = 1;
constexpr unsigned int maxDay = 7;

enum class RtcTimeContainerError
{
    Ok,
    InvalidDate,
    UnitializedContainer,
    UndefinedError,
    NumberOfErrors
};

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

    RtcTimeContainerError getContainerStatusWithoutDayVerification() const{
        if (yr < minYearWhenYearValueIsSmallerRtcIsUninitialized) {
            return RtcTimeContainerError::UnitializedContainer;
        }
        if (yr > maxYearWhenYearValueIsHigherRtcIsInvalid) {
            return RtcTimeContainerError::InvalidDate;
        }
        if (mon < minMonth || mon > maxMonth) {
            return RtcTimeContainerError::InvalidDate;
        }
        if (date < minDate || date > maxDate) {
            return RtcTimeContainerError::InvalidDate;
        }
        if (hr < minHour || hr > maxHour) {
            return RtcTimeContainerError::InvalidDate;
        }
        if (min < minMinute || min > maxMinute) {
            return RtcTimeContainerError::InvalidDate;
        }
        if (sec < minSecond || sec > maxSecond) {
            return RtcTimeContainerError::InvalidDate;
        }
        return RtcTimeContainerError::Ok;
    }

    RtcTimeContainerError getContainerStatus() const{
        RtcTimeContainerError status_without_day_verification = getContainerStatusWithoutDayVerification();
        if (status_without_day_verification != RtcTimeContainerError::Ok) {
            return status_without_day_verification;
        }
        if (day < minDay || day > maxDay) {
            return RtcTimeContainerError::InvalidDate;
        }
        return RtcTimeContainerError::Ok;
    }

    bool isTimeDateValid() const{
        return getContainerStatusWithoutDayVerification() == RtcTimeContainerError::Ok;
    }

    bool isTimeDateDayValid() const{
        return getContainerStatus() == RtcTimeContainerError::Ok;
    }
};

namespace ProjectTypes
{
    using RTC_Time = RtcTimeContainer;
}
