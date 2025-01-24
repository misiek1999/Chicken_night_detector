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
    Log.begin(LOG_LEVEL_INFO, &Serial);
    Log.noticeln("Log initialized");
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

// logCount [timestamp[ms]] filename:line_number - message
#define LOG_PREFIX "%u [%d] %s:%d- "
#define LOG_MS millis()
#ifndef __FILENAME__
constexpr const char* file_name(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path++ == '\\') {
            file = path;
        }
    }
    return file;
}
#define __FILENAME__ file_name(__FILE__)
#endif

#define LOG_ERROR(MSG, ...) Log.errorln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(MSG, ...) Log.warningln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_NOTICE(MSG, ...) Log.noticeln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(MSG, ...) Log.infoln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(MSG, ...) Log.traceln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
#define LOG_VERBOSE(MSG, ...) Log.verboseln(LOG_PREFIX MSG, log_number++, LOG_MS, __FILENAME__, __LINE__, ##__VA_ARGS__)
