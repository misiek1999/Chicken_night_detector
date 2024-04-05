#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>

enum class LogSource {
    UsbSerial,
    Serial1,
    UsbAndSerial1,
    NumberOfSerials
};

inline void initLog() {
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
    Log.notice("Log initialized");
}

inline bool changeLogLevel(const size_t &log_level) {
    if (log_level <= LOG_LEVEL_VERBOSE) {
        Log.setLevel(log_level);
        return true;
    }
    return false;
}

inline void chengeLogSource(const LogSource &log_source) {
    switch (log_source) {
        case LogSource::UsbSerial:
            Log.begin(LOG_LEVEL_NOTICE, &Serial);
            break;
        case LogSource::Serial1:
            Log.begin(LOG_LEVEL_NOTICE, &Serial1);
            break;
        // TODO: Implement this function
        // case LogSource::UsbAndSerial1:
        //     break;
        default:
            Log.begin(LOG_LEVEL_NOTICE, &Serial);
            break;
    }
}

inline size_t log_number = {};

// LogID [log type][timestamp[ms]] filename:line_number - message
#define LOG_PREFIX "%u [%s][%d] %s:%d - "
#define LOG_MS millis()
#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

#define LOG_ERROR(MSG, ...) Log.error(LOG_PREFIX MSG, log_number++, "D", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(MSG, ...) Log.warning(LOG_PREFIX MSG, log_number++, "W", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_NOTICE(MSG, ...) Log.notice(LOG_PREFIX MSG, log_number++, "N", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(MSG, ...) Log.info(LOG_PREFIX MSG, log_number++, "I", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(MSG, ...) Log.debug(LOG_PREFIX MSG, log_number++, "D", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_VERBOSE(MSG, ...) Log.verbose(LOG_PREFIX MSG, log_number++, "V", LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
