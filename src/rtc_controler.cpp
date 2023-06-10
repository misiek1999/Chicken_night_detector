#include "rtc_controler.h"

RtcSource rtc_source = RtcSource::Internal; // Default state

// Class with DS1302 RTC clock
DS1302 external_rtc(DS1302_EN_PIN, DS1302_DAT_PIN, DS1302_CLK_PIN);
// static Time current_time(2004, 1, 1, 0, 0, 0, Time::Day::kMonday);    // Initialize object with default values

void setExternalRtcTime(Time &time_to_set) {
    external_rtc.time(time_to_set);
        char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "set: %04d-%02d-%02d %02d:%02d:%02d",
    time_to_set.yr, time_to_set.mon, time_to_set.date,
    time_to_set.hr, time_to_set.min, time_to_set.sec);

    // Print the formatted string to serial so we can see the time.
     Serial.println(buf);
}

void getExternalRtcTime(Time &current_time) {
    Time current_time_ = external_rtc.time();
    char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "test: %04d-%02d-%02d %02d:%02d:%02d",
    current_time_.yr, current_time_.mon, current_time_.date,
    current_time_.hr, current_time_.min, current_time_.sec);

    // Print the formatted string to serial so we can see the time.
     Serial.println(buf);
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

void setRtcSource(RtcSource rtc_source_to_set) {
    //TODO: add verification of rtc_source_to_set
    rtc_source = rtc_source_to_set;
}

void initRtc(void) {
    // TODO: Debug purpose only
    external_rtc.writeProtect(false);
    external_rtc.halt(false);
}
