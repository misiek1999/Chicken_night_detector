#pragma once

#include <Arduino.h>
#include <ctime>
#include "project_pin_definition.h"
#include "project_types.h"
#include "rtc_adapter.h"
#include "external_rtc_adapter.h"
#include "internal_rtc_adapter.h"
#include "internal_and_external_sync_adapter.h"

enum class RtcSource {
    Internal,   // use only internal RTC clock
    External,   // use external RTC clock
    SyncExternalWithInternal,   // when power is supplied use internal RTC, however when power is not supplied use external RTC
    DCF77,      // use DCF77 module
    None,
    NumberOfRtcSources  // number of rtc sources
};

enum class RtcStatus {
    Ok,
    Uninitialized,
    Disconnected,
    UndefinedError
};

class RtcDriver {
private:
    RtcDriver() = default;
    RtcDriver(const RtcDriver &) = delete;
    explicit RtcDriver(RtcSource rtc_source_to_set);
 public:
    /*
        @details get instance of RtcDriver
    */
    static RtcDriver& getInstance() {
        static RtcDriver rtc_driver_inst(RtcSource::External);
        return rtc_driver_inst;
    }
    /*
        @details get current time from rtc
    */
    std::time_t getCurrentTimeRtc();

    /*
        @details set current time to rtc
        @param time_to_set time to set to rtc
        @return true if time is set successful, false otherwise
    */
    bool setTimeToRtc(const std::time_t &time);

    /*
        @details set rtc source
    */
    void setRtcSource(const RtcSource rtc_source_to_set);

    /*
        @details get rtc source
    */
    RtcSource getRtcSource();

    /*
        @details get rtc status
        @return RtcStatus enum with current status of rtc
    */
    RtcStatus getRtcStatus();

 private:
    RtcSource rtc_source_ = RtcSource::None;

    void setExternalRtcTime(const std::time_t &time);
    void getExternalRtcTime(std::time_t &time);
    void setInternalRtcTime(const std::time_t &time);
    void getInternalRtcTime(std::time_t &time);
    void setIntWithExtSyncRtcTime(const std::time_t &time);
    void getIntWithExtSyncRtcTime(std::time_t &time);

};

