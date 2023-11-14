#include "cli_callbacks.h"

#include <Arduino.h>
#include "light_control.h"
#include "rtc_driver.h"


// declaration of function to process command
void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setProgramModeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getProgramModeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);

etl::vector<CliCommandBinding, CLI::kMaxBindingCount> cli_callbacks = {{
        "set_rtc",                      // command name (spaces are not allowed)
        "set rtc time: yyyy-mm-dd-hh-mm-ss [space separated] PROVIDE TIME IN WINTER TIME!!!",   // Optional help for a command (NULL for no help)
        true,                       // flag whether to tokenize arguments (see below)
        nullptr,                    // optional pointer to any application context
        setRtcTimeCli               // binding function
    },
    {
        "get_rtc",                  // command name (spaces are not allowed)
        "get current rtc time: yyyy-mm-dd-hh-mm-ss",   // Optional help for a command (NULL for no help)
        false,                      // flag whether to tokenize arguments (see below)
        nullptr,                    // optional pointer to any application context
        getRtcTimeCli               // binding function
    },
    {            
        "rtc_source",                  // command name (spaces are not allowed)
        "set rtc source: 1 -Internal, 2 - external",   // Optional help for a command (NULL for no help)
        false,                      // flag whether to tokenize arguments (see below)
        nullptr,                    // optional pointer to any application context
        setRtcSourceCli               // binding function
    },
    {            
        "get_rtc_source",                  // command name (spaces are not allowed)
            "get rtc source",   // Optional help for a command (NULL for no help)
            false,                      // flag whether to tokenize arguments (see below)
            nullptr,                    // optional pointer to any application context
            getRtcSourceCli               // binding function
    },
    {            
        "set_program_mode",                  // command name (spaces are not allowed)
            "set program mode: 1 -RTC, 2 - Light sensor",   // Optional help for a command (NULL for no help)
            false,                      // flag whether to tokenize arguments (see below)
            nullptr,                    // optional pointer to any application context
            setProgramModeCli               // binding function
    },
    {            
        "get_program_mode",                  // command name (spaces are not allowed)
        "get program mode",   // Optional help for a command (NULL for no help)
        false,                      // flag whether to tokenize arguments (see below)
        nullptr,                    // optional pointer to any application context
        getProgramModeCli           // binding function
    },
    {            
        "get_light_status",                  // command name (spaces are not allowed)
        "get light status",   // Optional help for a command (NULL for no help)
        false,                      // flag whether to tokenize arguments (see below)
        nullptr,                    // optional pointer to any application context
        getLightStatusCli           // binding function
    }
};


void sendCharOverSerial(EmbeddedCli *embeddedCli, char c) {
    Serial.write(c);
}

bool checkCstringIsNumber(const char *str) {
    for (size_t i = 0; i < strlen(str); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void saveValueToTimeVariable(ProjectTypes::RTC_Time& time_to_set, uint32_t value, size_t index) {
    switch (index) {
        case 0:
            time_to_set.yr = value;
            break;
        case 1:
            time_to_set.mon = value;
            break;
        case 2:
            time_to_set.date = value;
            break;
        case 3:
            time_to_set.hr = value;
            break;
        case 4:
            time_to_set.min = value;
            break;
        case 5:
            time_to_set.sec = value;
            break;
        default:
            break;
    }
}

void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    // Time value
    ProjectTypes::RTC_Time time_to_set;
    // get number of tokens
    size_t token_count = embeddedCliGetTokenCount(args);
    if (token_count != 6) {
        Serial.println(F("Wrong number of arguments!"));
        return;
    }
    // get tokens
    for (size_t i = 1; i <= token_count; ++i) {
        // check if token is number
          const char *token = embeddedCliGetToken(args, i);
        if (!checkCstringIsNumber(token)) {
            Serial.println(F("Argument is not number!"));
            Serial.print(F("arg "));
            Serial.print((char) ('0' + i));
            Serial.print(F(": "));
            Serial.println(token);
            return;
        }
        // check if token is in range
        int32_t value = atoi(token);
        if (RtcDriver::checkRtcValueIsInRange(value, i - 1) == false) {
            Serial.println(F("Argument is not in range!"));
            Serial.print(F("arg "));
            Serial.print((char) ('0' + i));
            Serial.print(F(": "));
            Serial.println(token);
            return;
        }
        // save value to time
        saveValueToTimeVariable(time_to_set, value, i - 1);
    }
    // set rtc time
    rtc_driver.setTimeToRtc(time_to_set);
    Serial.println(F("New time is set!"));
}

void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    // read current time
    ProjectTypes::RTC_Time currentTime;
    rtc_driver.getCurrentTimeRtc(currentTime);
    // print current time
    char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "Time: %04d-%02d-%02d %02d:%02d:%02d",
        currentTime.yr, currentTime.mon, currentTime.date,
        currentTime.hr, currentTime.min, currentTime.sec);
    // Print the formatted string to serial so we can see the time.
    Serial.println(buf);
    RtcStatus rtc_status = rtc_driver.getRtcStatus();
    if (rtc_status == RtcStatus::Disconnected)
        Serial.println("Sensor is disconnected!");
    else if (rtc_status == RtcStatus::Uninitialized)
        Serial.println("Sensor is uninitialized!");
}

void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {

}

void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    RtcSource source = rtc_driver.getRtcSource();
    if (source == RtcSource::External) {
        Serial.println("External rtc source");
        return;
    }
    if (source == RtcSource::Internal) {
        Serial.println("Internal rtc source");
        return;
    }
}

void setProgramModeCli(EmbeddedCli *embeddedCli, char *args, void *context) {

}

void getProgramModeCli(EmbeddedCli *embeddedCli, char *args, void *context) {

}

void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    // LightControl::LightState status = getLightState();
    // if (status == LightControl::LightState::On) {
    //     Serial.println("Light is on");
    //     return;
    // }
    // if (status == LightControl::LightState::Off) {
    //     Serial.println("Light is off");
    //     return;
    // }
    // if (status == LightControl::LightState::Blanking) {
    //     Serial.println("Light is Blanking");
    //     return;
    // }
}
