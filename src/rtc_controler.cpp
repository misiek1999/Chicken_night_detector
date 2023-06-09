#include "rtc_controler.h"

RtcSource rtc_source = RtcSource::Internal; // Default state

// Class with DS1302 RTC clock
DS1302 external_rtc(DS1302_EN_PIN, DS1302_CLK_PIN, DS1302_DAT_PIN);
Time current_time(2000, 1, 1, 0, 0, 0, Time::Day::kMonday);    // Initialize object with default values

void setExternalRtcTime(Time &time_to_set) {
    external_rtc.time(time_to_set);
}

void getExternalRtcTime(Time &current_time) {
    current_time = external_rtc.time();
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
    pinMode(DS1302_RST_PIN, OUTPUT);
    digitalWrite(DS1302_RST_PIN, HIGH);
    // TODO: Debug purpose only
    external_rtc.writeProtect(false);
    external_rtc.halt(false);
}
