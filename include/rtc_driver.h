#pragma once

#include <Arduino.h>
#include "project_pin_definition.h"
#include "project_types.h"
#include "rtc_adapter.h"
#include "external_rtc_adapter.h"
#include "internal_rtc_adapter.h"
#include "internal_and_external_sync_adapter.h"

enum class RtcSource
{
    Internal,   // use only internal RTC clock
    External,   // use external RTC clock
    SyncExternalWithInternal,   // when power is supplied use internal RTC, however when power is not supplied use external RTC
    DCF77,      // use DCF77 module
    None,
    NumberOfRtcSources  // number of rtc sources
};

enum class RtcStatus
{
    Ok,
    Uninitialized,
    Disconnected,
    UndefinedError
};

class RtcDriver
{
public:
    RtcDriver();
    RtcDriver(RtcSource rtc_source_to_set);
    /*
        @details get current time from rtc
    */
    void getCurrentTimeRtc(ProjectTypes::RTC_Time &current_time);

    /*
        @details set current time to rtc
        @param time_to_set time to set to rtc
        @return true if time is set successful, false otherwise
    */
    bool setTimeToRtc(ProjectTypes::RTC_Time &time_to_set);

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
    RtcStatus getRtcStatus(void);

private:
    RtcSource rtc_source_ = RtcSource::None;

    void setExternalRtcTime(ProjectTypes::RTC_Time &time_to_set);
    void getExternalRtcTime(ProjectTypes::RTC_Time &current_time);
    void setInternalRtcTime(ProjectTypes::RTC_Time &time_to_set);
    void getInternalRtcTime(ProjectTypes::RTC_Time &current_time);
    void setIntWithExtSyncRtcTime(ProjectTypes::RTC_Time &time_to_set);
    void getIntWithExtSyncRtcTime(ProjectTypes::RTC_Time &current_time);

};

extern RtcDriver rtc_driver;
