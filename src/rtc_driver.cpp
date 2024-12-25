#include "rtc_driver.h"
#include "string"
#include "log.h"
#include "project_const.h"

constexpr auto kMinValidYear = 2020 - ProjectConst::kTmStructInitYear;
constexpr auto kMaxValidYear = 2100 - ProjectConst::kTmStructInitYear;

// TODO: implement this functions
// TODO: chang implememnntation to class inheritance
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

void RtcDriver::reportRtcError() const {
    auto* error_manager = SystemControl::ErrorManager::getInstance();
    error_manager->setError(SystemControl::ErrorCode::kRtcTimeNotSet);
}

RtcDriver::RtcDriver(RtcSource rtc_source_to_set):
    rtc_source_(rtc_source_to_set) {
    // initialize external rtc module
    ModuleAdapter::init_external_rtc_module();
}

std::time_t RtcDriver::getTimeFromRtc() {
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
    if (!checkRtcTimeIsValid(time)) {
        reportRtcError();
        LOG_WARNING("Rtc time is not valid");
    }
    return time;
}

bool RtcDriver::setTimeToRtc(const std::time_t &time) {
    LOG_INFO("Set time to rtc: %s", std::to_string(time).c_str());
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
        LOG_INFO("Rtc source set to: %d", rtc_source_to_set);
    } else {
        LOG_ERROR("Invalid rtc source: %d", rtc_source_to_set);
    }
    if (checkRtcIsOk() != RtcStatus::Ok) {
        LOG_ERROR("Rtc is not set correctly");
    }
}

RtcSource RtcDriver::getRtcSource() {
    return rtc_source_;
}

RtcStatus RtcDriver::checkRtcIsOk() {
    // read current time from rtc
    std::time_t time_from_sensor = getTimeFromRtc();
    // check if time is valid
    auto status = RtcStatus::Ok;
    // in case of error, notify error manager
    if (!checkRtcTimeIsValid(time_from_sensor)) {
        status = RtcStatus::Uninitialized;
        // set error code in error manager
        reportRtcError();
    }
    return status;
}

bool RtcDriver::checkRtcTimeIsValid(const std::time_t &time) {
    bool status = true;
    const std::tm date = *std::localtime(&time);
    if (time == 0) {
        status =  false;
    }
    if (date.tm_year < kMinValidYear || date.tm_year > kMaxValidYear) {
        status = false;
    }
    return status;
}
