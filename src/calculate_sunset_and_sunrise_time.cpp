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
    DaytimeCalculator::init_time_zone_value),
    last_input_date{} {
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
    sun_set(latitude, longitude, time_zone),
    last_input_date{} {
}

std::time_t DaytimeCalculator::getSunsetTime(const std::time_t & input_time) {
    processSunriseAndSunsetTime(input_time);
    return this->sunset_time;
}

std::time_t DaytimeCalculator::getSunriseTime(const std::time_t & input_time) {
    processSunriseAndSunsetTime(input_time);
    return this->sunrise_time;
}

void DaytimeCalculator::processSunriseAndSunsetTime(const std::time_t & input_time) {
    if (checkDataHasChanged(input_time)) {
        updateSunsetAndSunriseTime(input_time);
    }
}

bool DaytimeCalculator::checkDataHasChanged(const std::time_t & input_time) const {
    bool date_has_changed = false;
    const auto *const calendar_date = std::localtime(&input_time);
    if (calendar_date->tm_year != last_input_date.tm_year ||
        calendar_date->tm_mon  != last_input_date.tm_mon ||
        calendar_date->tm_mday != last_input_date.tm_mday) {
        date_has_changed = true;
    }
    return date_has_changed;
}

void DaytimeCalculator::updateSunsetAndSunriseTime(const std::time_t & input_time) {
    const auto *const calendar_date = std::localtime(&input_time);
    sun_set.setCurrentDate(calendar_date->tm_year, calendar_date->tm_mon, calendar_date->tm_yday);
    this->sunset_time = calcSunsetTime(input_time);
    this->sunrise_time = calcSunriseTime(input_time);
    this->last_input_date = *calendar_date;
}

std::time_t DaytimeCalculator::convertTimePastMidnightToCTime(const std::time_t & input_time, const ProjectTypes::abs_min_past_midnight_t & time_past_midnight) {
    auto daytime = *std::localtime(&input_time);
    daytime.tm_hour = time_past_midnight / 60;
    daytime.tm_min = time_past_midnight % 60;
    return std::mktime(&daytime);
}

std::time_t DaytimeCalculator::calcSunsetTime(const std::time_t & input_time) {
    auto min_past_midnight = static_cast<int>(sun_set.calcCivilSunset());
    return convertTimePastMidnightToCTime(input_time, min_past_midnight);
}

std::time_t DaytimeCalculator::calcSunriseTime(const std::time_t & input_time) {
    auto min_past_midnight = static_cast<int>(sun_set.calcCivilSunrise());
    return convertTimePastMidnightToCTime(input_time, min_past_midnight);
}
