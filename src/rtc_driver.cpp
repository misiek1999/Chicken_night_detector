#include "rtc_driver.h"

// TODO: implement this functions
void RtcDriver::setExternalRtcTime(const std::time_t &time) {
    ModuleAdapter::set_external_rtc_time(time);
}

void RtcDriver::getExternalRtcTime(std::time_t &time) {
    ModuleAdapter::get_external_rtc_time(time);
}

void RtcDriver::setInternalRtcTime(const std::time_t &time) {
    ModuleAdapter::set_internal_rtc_time(time);
}

void RtcDriver::getInternalRtcTime(std::time_t &time) {
    ModuleAdapter::get_internal_rtc_time(time);
}

void RtcDriver::setIntWithExtSyncRtcTime(const std::time_t &time) {
    ModuleAdapter::set_inter_and_exter_sync_rtc_time(time);
}

void RtcDriver::getIntWithExtSyncRtcTime(std::time_t &time) {
    ModuleAdapter::get_inter_and_exter_sync_rtc_time(time);
}

RtcDriver::RtcDriver(RtcSource rtc_source_to_set):
    rtc_source_(rtc_source_to_set) {
}

std::time_t RtcDriver::getCurrentTimeRtc() {
    std::time_t time = {};
    switch (rtc_source_) {
    case RtcSource::Internal:
        getInternalRtcTime(time);
        break;
    case RtcSource::External:
        getExternalRtcTime(time);
        break;
    case RtcSource::SyncExternalWithInternal:
        getIntWithExtSyncRtcTime(time);
        break;
    default:
        break;
    }
    return time;
}

bool RtcDriver::setTimeToRtc(const std::time_t &time) {
    // set time to selected rtc
    switch (rtc_source_) {
    case RtcSource::Internal:
        setInternalRtcTime(time);
        break;
    case RtcSource::External:
        setExternalRtcTime(time);
        break;
    case RtcSource::SyncExternalWithInternal:
        setIntWithExtSyncRtcTime(time);
        break;
    default:
        break;
    }
    return true;
}

void RtcDriver::setRtcSource(const RtcSource rtc_source_to_set) {
    if (static_cast<int>(rtc_source_to_set) >= 0 && rtc_source_to_set < RtcSource::NumberOfRtcSources) {
        rtc_source_ = rtc_source_to_set;
    }
}

RtcSource RtcDriver::getRtcSource() {
    return rtc_source_;
}

RtcStatus RtcDriver::getRtcStatus() {
    // read current time from rtc
    std::time_t time_from_sensor = getCurrentTimeRtc();
    std::tm date = *std::localtime(&time_from_sensor);
    // check if time is valid
    if (time_from_sensor == 0) {
        return RtcStatus::Uninitialized;
    }
    if (date.tm_year < 2020 || date.tm_year > 2100) {
        return RtcStatus::Uninitialized;
    }
    return RtcStatus::Ok;
}
