#include "external_rtc_adapter.h"
#include "DS1302.h"

namespace ModuleAdapter {
// Class with DS1302 RTC clock
DS1302 external_rtc(kPinDS1302En, kPinDS1302Data, kPinDS1302Clk);

// Local function declaration
void convert_internal_rtc_format_to_external_rtc_format(const std::time_t &time_to_convert, Time &converted_time) {
    auto date = *std::localtime(&time_to_convert);
    converted_time.yr = date.tm_year;
    converted_time.mon = date.tm_mon;
    converted_time.day = static_cast<Time::Day>(date.tm_wday);
    converted_time.hr = date.tm_hour;
    converted_time.min = date.tm_min;
    converted_time.sec = date.tm_sec;
    converted_time.date = date.tm_mday;
}

void convert_external_rtc_format_to_internal_rtc_format(const Time &time_to_convert, std::time_t &converted_time) {
    std::tm input_date = {};
    input_date.tm_year = time_to_convert.yr;
    input_date.tm_mon = time_to_convert.mon;
    input_date.tm_wday = static_cast<ProjectTypes::RTC_Time::Day>(time_to_convert.day);
    input_date.tm_hour = time_to_convert.hr;
    input_date.tm_min = time_to_convert.min;
    input_date.tm_sec = time_to_convert.sec;
    input_date.tm_mday = time_to_convert.date;
    converted_time = std::mktime(&input_date);
}



// Global function definitions

void init_external_rtc_module() {
    external_rtc.writeProtect(false);
    external_rtc.halt(false);
}

void get_external_rtc_time(std::time_t &time) {
    // read time from external rtc
    const Time ds1302_time = external_rtc.time();
    // convert time format
    convert_external_rtc_format_to_internal_rtc_format(ds1302_time, time);
}

void set_external_rtc_time(const std::time_t &time) {
    // convert rtc time format
    Time ds1302_time(0, 0, 0, 0, 0, 0, Time::Day::kMonday);
    convert_internal_rtc_format_to_external_rtc_format(time, ds1302_time);
    // set new time to external rtc
    external_rtc.time(ds1302_time);
}

}  //  namespace ModuleAdapter
