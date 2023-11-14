#pragma once

#include <Arduino.h>
#include <project_types.h>
#include <rtc_time_container.h>
#include <sunset.h>

class DaytimeCalculator
{
public:
    DaytimeCalculator();
    DaytimeCalculator(const ProjectTypes::latitude_t &latitude,
                      const ProjectTypes::longitude_t &longitude,
                      const ProjectTypes::time_zone_t &time_zone,
                      const ProjectTypes::req_t &req);

    ProjectTypes::abs_min_past_midnight_t getSunsetTime(const ProjectTypes::RTC_Time &rtc_time);
    ProjectTypes::abs_min_past_midnight_t getSunriseTime(const ProjectTypes::RTC_Time &rtc_time);

private:
    ProjectTypes::abs_min_past_midnight_t sunset_time;
    ProjectTypes::abs_min_past_midnight_t sunrise_time;
    ProjectTypes::RTC_Time last_rtc_time;
    ProjectTypes::latitude_t latitude;
    ProjectTypes::longitude_t longitude;
    ProjectTypes::time_zone_t time_zone;
    ProjectTypes::req_t req;
    SunSet sun_set;

    static constexpr ProjectTypes::abs_min_past_midnight_t init_minute_value = 0;
    static constexpr ProjectTypes::latitude_t init_latitude_value = 0;
    static constexpr ProjectTypes::longitude_t init_longitude_value = 0;
    static constexpr ProjectTypes::time_zone_t init_time_zone_value = 0;
    static constexpr ProjectTypes::req_t init_req_value = 0;

    void processSunriseAndSunsetTime(const ProjectTypes::RTC_Time &rtc_time);
    bool checkDataHasChanged(const ProjectTypes::RTC_Time &rtc_time) const;
    void updateSunsetAndSunriseTime(const ProjectTypes::RTC_Time &rtc_time);
};

