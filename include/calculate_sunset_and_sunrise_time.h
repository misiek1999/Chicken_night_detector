#pragma once

#include <Arduino.h>
#include <ctime>
#include <project_types.h>
#include <sunset.h>

class DaytimeCalculator {
 public:
    DaytimeCalculator();
    DaytimeCalculator(const ProjectTypes::latitude_t &latitude,
                      const ProjectTypes::longitude_t &longitude,
                      const ProjectTypes::time_zone_t &time_zone,
                      const ProjectTypes::req_t &req);

    std::time_t getSunsetTime(const std::time_t & input_time);
    std::time_t getSunriseTime(const std::time_t & input_time);

 private:
    std::time_t sunset_time;
    std::time_t sunrise_time;
    ProjectTypes::latitude_t latitude;
    ProjectTypes::longitude_t longitude;
    ProjectTypes::time_zone_t time_zone;
    ProjectTypes::req_t req;
    SunSet sun_set;
    std::tm last_input_date;

    static constexpr ProjectTypes::abs_min_past_midnight_t init_minute_value = 0;
    static constexpr ProjectTypes::latitude_t init_latitude_value = 0;
    static constexpr ProjectTypes::longitude_t init_longitude_value = 0;
    static constexpr ProjectTypes::time_zone_t init_time_zone_value = 0;
    static constexpr ProjectTypes::req_t init_req_value = 0;

    void processSunriseAndSunsetTime(const std::time_t & input_time);
    bool checkDataHasChanged(const std::time_t & input_time) const;
    void updateSunsetAndSunriseTime(const std::time_t & input_time);
};

