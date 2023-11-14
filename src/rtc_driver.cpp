#include "rtc_driver.h"

RtcDriver rtc_driver;

void RtcDriver::setExternalRtcTime(ProjectTypes::RTC_Time &time_to_set) {
    ModuleAdapter::set_external_rtc_time(time_to_set);
}

void RtcDriver::getExternalRtcTime(ProjectTypes::RTC_Time &current_time) {
    ModuleAdapter::get_external_rtc_time(current_time);
}

void RtcDriver::setInternalRtcTime(ProjectTypes::RTC_Time &time_to_set) {
    //TODO: Implement function
}

void RtcDriver::getInternalRtcTime(ProjectTypes::RTC_Time &current_time) {
    //TODO: Implement function
    
}

RtcDriver::RtcDriver()
{
}

void RtcDriver::getCurrentTimeRtc(ProjectTypes::RTC_Time &current_time) {
    switch (rtc_source_)
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

void RtcDriver::setTimeToRtc(ProjectTypes::RTC_Time &time_to_set) {
    switch (rtc_source_)
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

void RtcDriver::setRtcSource(const RtcSource rtc_source_to_set) {
    //TODO: add verification of rtc_source_to_set
    rtc_source_ = rtc_source_to_set;
}

RtcSource RtcDriver::getRtcSource()
{
    return rtc_source_;
}


RtcStatus RtcDriver::getRtcStatus(void) {
    // read current time from rtc
    ProjectTypes::RTC_Time time_from_sensor;
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

bool RtcDriver::checkRtcValueIsInRange(uint32_t value, size_t index) {
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

