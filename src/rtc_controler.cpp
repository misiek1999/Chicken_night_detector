#include "rtc_controler.h"

static RtcSource rtc_source = RtcSource::Internal; // Default state

// Class with DS1302 RTC clock
DS1302 external_rtc(DS1302_EN_PIN, DS1302_DAT_PIN, DS1302_CLK_PIN);

void setExternalRtcTime(Time &time_to_set) {
    external_rtc.time(time_to_set);
}

void getExternalRtcTime(Time &current_time) {
    // read time from external rtc
    Time time_from_sensor = external_rtc.time();
    // copy values to input argument
    current_time.yr = time_from_sensor.yr;
    current_time.mon = time_from_sensor.mon;
    current_time.day = time_from_sensor.day;
    current_time.hr = time_from_sensor.hr;
    current_time.min = time_from_sensor.min;
    current_time.sec = time_from_sensor.sec;
    current_time.date = time_from_sensor.date;
}

void setInternalRtcTime(Time &time_to_set) {
    //TODO: Implement function
}

void getInternalRtcTime(Time &current_time) {
    //TODO: Implement function
}

void getCurrentTimeRtc(Time &current_time) {
    switch (rtc_source)
    {
    case RtcSource::Internal:
        getInternalRtcTime(current_time);
        break;
    case RtcSource::External:
        getExternalRtcTime(current_time);
        break;
    default:
        break;
    }
}

void setTimeToRtc(Time &time_to_set) {
    switch (rtc_source)
    {
    case RtcSource::Internal:
        setInternalRtcTime(time_to_set);
        break;
    case RtcSource::External:
        setExternalRtcTime(time_to_set);
        break;
    default:
        break;
    }
}

void setRtcSource(const RtcSource rtc_source_to_set) {
    //TODO: add verification of rtc_source_to_set
    rtc_source = rtc_source_to_set;
}

RtcSource getRtcSource()
{
    return rtc_source;
}

void initRtc(void) {
    // TODO: Debug purpose only
    external_rtc.writeProtect(false);
    external_rtc.halt(false);
}

RtcStatus getRtcStatus(void) {
    // read current time from rtc
    Time time_from_sensor(0, 0, 0, 0, 0, 0, (Time::Day)0);
    getCurrentTimeRtc(time_from_sensor);
    // check if time is valid
    if (time_from_sensor.yr < minYearWhenYearValueIsSmallerRtcIsUninitialized) {
        return RtcStatus::Uninitialized;
    }
    if (time_from_sensor.yr > maxYearWhenYearValueIsHigherRtcIsDisconnected) {
        return RtcStatus::Disconnected;
    }
    return RtcStatus::Ok;
}

bool checkRtcValueIsInRange(uint32_t value, size_t index) {
    bool status = false;    // default value
    switch (index) {
    case 0: // year
        if(value > 2000 && value < 2100)
            status = true;
        break;
    case 1: // month
        if(value > 0 && value < 13)
            status = true;
        break;
    case 2: // day
        if(value > 0 && value < 32)
            status = true;
        break;
    case 3: // hour
        if(value < 24)
            status = true;
        break;
    case 4: // minute
    case 5: // second
        if(value < 60)
            status = true;
        break;
    default:
        break;
    }

    return status;
}
