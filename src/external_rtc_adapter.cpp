#include "external_rtc_adapter.h"
#include "DS3231.h"
#include <wire.h>
#include "project_const.h"

namespace ModuleAdapter {
// Class with DS3231 RTC clock
DS3231* getExternalRtc() {
    static DS3231 external_rtc;
    return &external_rtc;
}

bool kCentury = false;
bool kH24 = false;
bool kAM = false;

// Global function definitions
void init_external_rtc_module() {
    auto *external_rtc = getExternalRtc();
    external_rtc->setClockMode(false);
}

void get_external_rtc_time(std::time_t &time) {
    auto *external_rtc = getExternalRtc();
    std::tm tm_date = {};
    tm_date.tm_year = external_rtc->getYear();
    tm_date.tm_mon = external_rtc->getMonth(kCentury) - ProjectConst::kMonthSyncOffset;
    tm_date.tm_mday = external_rtc->getDate();
    tm_date.tm_hour = external_rtc->getHour(kH24, kAM);
    tm_date.tm_min = external_rtc->getMinute();
    tm_date.tm_sec = external_rtc->getSecond();
    time = std::mktime(&tm_date);
}

void set_external_rtc_time(const std::time_t &time) {
    auto *external_rtc = getExternalRtc();
    // set new time to external rtc
    external_rtc->setEpoch(time);
}

}  //  namespace ModuleAdapter
