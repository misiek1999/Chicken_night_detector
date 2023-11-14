#include "calculate_sunset_and_sunrise_time.h"
#include "project_const.h"

DaytimeCalculator::DaytimeCalculator():
    sunset_time(DaytimeCalculator::init_minute_value),
    sunrise_time(DaytimeCalculator::init_minute_value),
    latitude(DaytimeCalculator::init_latitude_value),
    longitude(DaytimeCalculator::init_longitude_value),
    time_zone(DaytimeCalculator::init_time_zone_value),
    req(DaytimeCalculator::init_req_value),
    sun_set(DaytimeCalculator::init_latitude_value, DaytimeCalculator::init_longitude_value,
        DaytimeCalculator::init_time_zone_value)
{
}

DaytimeCalculator::DaytimeCalculator(const ProjectTypes::latitude_t &latitude,
                                     const ProjectTypes::longitude_t &longitude,
                                     const ProjectTypes::time_zone_t &time_zone,
                                     const ProjectTypes::req_t &req):
    sunset_time(DaytimeCalculator::init_minute_value),
    sunrise_time(DaytimeCalculator::init_minute_value),
    latitude(latitude),
    longitude(longitude),
    time_zone(time_zone),
    req(req),
    sun_set(latitude, longitude, time_zone)
{
}

ProjectTypes::abs_min_past_midnight_t DaytimeCalculator::getSunsetTime(const ProjectTypes::RTC_Time & rtc_time)
{
    processSunriseAndSunsetTime(rtc_time);
    return this->sunset_time;
}

ProjectTypes::abs_min_past_midnight_t DaytimeCalculator::getSunriseTime(const ProjectTypes::RTC_Time & rtc_time)
{
    processSunriseAndSunsetTime(rtc_time);
    return this->sunrise_time;
}

void DaytimeCalculator::processSunriseAndSunsetTime(const ProjectTypes::RTC_Time & rtc_time)
{
    if (checkDataHasChanged(rtc_time)) {
        updateSunsetAndSunriseTime(rtc_time);
    }
}

bool DaytimeCalculator::checkDataHasChanged(const ProjectTypes::RTC_Time & rtc_time) const
{
    bool date_has_changed = false;
    if (rtc_time.yr != this->last_rtc_time.yr ||
        rtc_time.mon != this->last_rtc_time.mon ||
        rtc_time.day != this->last_rtc_time.day) {
        date_has_changed = true;
    }
    return date_has_changed;
}

void DaytimeCalculator::updateSunsetAndSunriseTime(const ProjectTypes::RTC_Time & rtc_time)
{
    sun_set.setCurrentDate(rtc_time.yr, rtc_time.mon, rtc_time.day);
    this->sunset_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(sun_set.calcCivilSunset());
    this->sunrise_time = static_cast<ProjectTypes::abs_min_past_midnight_t>(sun_set.calcCivilSunrise());
    this->last_rtc_time = rtc_time;
}
