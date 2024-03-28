#pragma once

#include <Arduino.h>
#include <ctime>
#include "rtc_adapter.h"
#include "project_pin_definition.h"
#include "project_types.h"
#include "rtc_time_container.h"

namespace ModuleAdapter {
    /*
        @details function to initialize module
    */
    void init_external_rtc_module();

    /*
        @details get current time from external rtc
    */
    void get_external_rtc_time(std::time_t &time);

    /*
        @details set current time to external rtc
    */
   void set_external_rtc_time(const std::time_t &time);
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

}  //  namespace ModuleAdapter
