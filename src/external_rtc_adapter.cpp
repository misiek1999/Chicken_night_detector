#include "external_rtc_adapter.h"
#include "DS1302.h"

namespace ModuleAdapter
{
// Class with DS1302 RTC clock
DS1302 external_rtc(kPinDS1302En, kPinDS1302Data, kPinDS1302Clk);

// Local function declaration
void convert_internal_rtc_format_to_external_rtc_format(const ProjectTypes::RTC_Time &time_to_convert, Time &converted_time) {
    converted_time.yr = time_to_convert.yr;
    converted_time.mon = time_to_convert.mon;
    converted_time.day = static_cast<Time::Day>(time_to_convert.day);
    converted_time.hr = time_to_convert.hr;
    converted_time.min = time_to_convert.min;
    converted_time.sec = time_to_convert.sec;
    converted_time.date = time_to_convert.date;
}

void convert_external_rtc_format_to_internal_rtc_format(const Time &time_to_convert, ProjectTypes::RTC_Time &converted_time) {
    converted_time.yr = time_to_convert.yr;
    converted_time.mon = time_to_convert.mon;
    converted_time.day = static_cast<ProjectTypes::RTC_Time::Day>(time_to_convert.day);
    converted_time.hr = time_to_convert.hr;
    converted_time.min = time_to_convert.min;
    converted_time.sec = time_to_convert.sec;
    converted_time.date = time_to_convert.date;
}



// Global function definitions

void init_external_rtc_module(void) {
    external_rtc.writeProtect(false);
    external_rtc.halt(false);
}

void get_external_rtc_time(ProjectTypes::RTC_Time &time_from_external_rtc) {
    // read time from external rtc
    Time ds1302_time = external_rtc.time();
    // convert time format
    convert_external_rtc_format_to_internal_rtc_format(ds1302_time, time_from_external_rtc);
}

void set_external_rtc_time(const ProjectTypes::RTC_Time & time_to_set){
    // convert rtc time format
    Time ds1302_time(0,0,0,0,0,0, Time::Day::kMonday);
    convert_internal_rtc_format_to_external_rtc_format(time_to_set, ds1302_time);
    // set new time to external rtc
    external_rtc.time(ds1302_time);
}

// void ExternalRtcModuleAdapter::initRtcModule(void) {
//     external_rtc.writeProtect(false);
//     external_rtc.halt(false);
// }

// void ExternalRtcModuleAdapter::getRtcTime(ProjectTypes::RTC_Time &time_from_external_rtc) {
//     // read time from external rtc
//     Time ds1302_time = external_rtc.time();
//     // convert time format
//     convert_external_rtc_format_to_internal_rtc_format(ds1302_time, time_from_external_rtc);
// }

// void ExternalRtcModuleAdapter::setRtcTime(const ProjectTypes::RTC_Time &time_to_set) {
//     // convert rtc time format
//     Time ds1302_time(0,0,0,0,0,0, Time::Day::kMonday);
//     convert_internal_rtc_format_to_external_rtc_format(time_to_set, ds1302_time);
//     // set new time to external rtc
//     external_rtc.time(ds1302_time);
// }

} // namespace ModuleAdapter
