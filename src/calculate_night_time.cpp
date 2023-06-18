#include "calculate_night_time.h"

SunSet sun_set(ProjectConst::kInstallationLatitude, ProjectConst::kInstallationLongitude, ProjectConst::kInstallationTimeZone);

bool detectNightUsingRtc(void)
{
    bool is_night_time = false;
    Time current_time(0, 0, 0, 0, 0, 0, (Time::Day)0);  // create time variable
    getCurrentTimeRtc(current_time);   // read time from rtc
    // set current date to sun_set class
    sun_set.setCurrentDate(current_time.yr, current_time.mon, current_time.day);
    // calculate sunset and sunrise time
    double sun_set_min = sun_set.calcCivilSunset();
    double sun_rise_min = sun_set.calcCivilSunrise();
    // turn on light before sunset
    sun_set_min -= ProjectConst::kMinutesToTurnOnLightBeforeSunset;
    // convert current time to minutes after midnight
    double current_time_min = current_time.hr * 60 + current_time.min;
    // check if current time is in night time
    if (current_time_min < sun_rise_min || current_time_min > sun_set_min - ProjectConst::kMinutesToTurnOnLightBeforeSunset) {
        is_night_time = true;
    }
    else {
        is_night_time = false;
    }
    return is_night_time;
}
