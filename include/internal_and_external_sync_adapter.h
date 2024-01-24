#pragma once

#include <Arduino.h>
#include "internal_rtc_adapter.h"
#include "external_rtc_adapter.h"

namespace ModuleAdapter
{
    /*
        @details function to initialize module
    */
    void init_inter_and_exter_sync_rtc_module(void);

    /*
        @details get current time from sync rtc
    */
    void get_inter_and_exter_sync_rtc_time(ProjectTypes::RTC_Time &time_from_external_rtc);

    /*
        @details set current time to sync rtc
    */
   void set_inter_and_exter_sync_rtc_time(const ProjectTypes::RTC_Time &time_to_set);
// class ExternalRtcModuleAdapter : public RtcModuleAdapter {
// public:
//     /*
//         @details function to initialize module
//     */
//     virtual void initRtcModule(void) override;

//     /*
//         @details get time from rtc module
//     */
//     virtual void getRtcTime(ProjectTypes::RTC_Time &time_from_external_rtc) override;

//     /*
//         @details set time to rtc module
//     */
//    virtual void setRtcTime(const ProjectTypes::RTC_Time &time_to_set) override;
// };

} // namespace ModuleAdapter

