#include "rtc_driver.h"

RtcDriver rtc_driver(RtcSource::Internal);

void RtcDriver::setExternalRtcTime(ProjectTypes::RTC_Time &time_to_set) {
    ModuleAdapter::set_external_rtc_time(time_to_set);
}

void RtcDriver::getExternalRtcTime(ProjectTypes::RTC_Time &current_time) {
    ModuleAdapter::get_external_rtc_time(current_time);
}

void RtcDriver::setInternalRtcTime(ProjectTypes::RTC_Time &time_to_set) {
    ModuleAdapter::set_internal_rtc_time(time_to_set);
}

void RtcDriver::getInternalRtcTime(ProjectTypes::RTC_Time &current_time) {
    ModuleAdapter::get_internal_rtc_time(current_time);
}

void RtcDriver::setIntWithExtSyncRtcTime(ProjectTypes::RTC_Time &time_to_set)
{
    ModuleAdapter::set_inter_and_exter_sync_rtc_time(time_to_set);
}

void RtcDriver::getIntWithExtSyncRtcTime(ProjectTypes::RTC_Time &current_time)
{
    ModuleAdapter::get_inter_and_exter_sync_rtc_time(current_time);
}

RtcDriver::RtcDriver()
{
}

RtcDriver::RtcDriver(RtcSource rtc_source_to_set):
    rtc_source_(rtc_source_to_set)
{
}


void RtcDriver::getCurrentTimeRtc(ProjectTypes::RTC_Time &current_time) {
    switch (rtc_source_) {
    case RtcSource::Internal:
        getInternalRtcTime(current_time);
        break;
    case RtcSource::External:
        getExternalRtcTime(current_time);
        break;
    case RtcSource::SyncExternalWithInternal:
        getIntWithExtSyncRtcTime(current_time);
        break;
    default:
        break;
    }
}

bool RtcDriver::setTimeToRtc(ProjectTypes::RTC_Time &time_to_set) {
    // check time is valid
    if (time_to_set.isTimeDateValid() == false) {
        return false;
    }
    // set time to selected rtc
    switch (rtc_source_) {
    case RtcSource::Internal:
        setInternalRtcTime(time_to_set);
        break;
    case RtcSource::External:
        setExternalRtcTime(time_to_set);
        break;
    case RtcSource::SyncExternalWithInternal:
        setIntWithExtSyncRtcTime(time_to_set);
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
    ProjectTypes::RTC_Time time_from_sensor;
    getCurrentTimeRtc(time_from_sensor);
    // check if time is valid
    RtcTimeContainerError rtc_time_container_error = time_from_sensor.getContainerStatusWithoutDayVerification();
    if (rtc_time_container_error == RtcTimeContainerError::Ok) {
        return RtcStatus::Ok;
    }
    if (rtc_time_container_error == RtcTimeContainerError::UnitializedContainer) {
        return RtcStatus::Uninitialized;
    }
    if (rtc_time_container_error == RtcTimeContainerError::InvalidDate) {
        return RtcStatus::Disconnected;
    }
    return RtcStatus::UndefinedError;
}
