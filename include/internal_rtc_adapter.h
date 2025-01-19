#pragma once

#include "rtc_adapter.h"
#include <ctime>
namespace ModuleAdapter {
    /*
        @details function to initialize module
    */
    void init_internal_rtc_module();

    /*
        @details get current time from internal rtc
    */
    void get_internal_rtc_time(std::time_t &time_from_internal_rtc);

    /*
        @details set current time to internal rtc
    */
   void set_internal_rtc_time(const std::time_t &time_to_set);

// class InternalRtcModuleAdapter : public RtcModuleAdapter {
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

}
