#include "cli_callbacks.h"

#include <Arduino.h>
#include <cstdlib>
#include "light_state.h"
#include "light_controller_process.h"
#include "rtc_driver.h"

// declaration of helper function
bool checkInputRtcArgumentValueIsInRange(uint32_t value, size_t index);

// declaration of function to process command
void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);

etl::array<CliCommandBinding, CLI::kMaxBindingCount> CLI::cli_callbacks = {{{
        "set_rtc",                      // command name (spaces are not allowed)
        "set rtc time: yyyy-mm-dd-hh-mm-ss [space separated] PROVIDE TIME IN WINTER TIME!!!",   // Optional help for a command (NULL for no help)
        true,                           // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        setRtcTimeCli                   // binding function
    },
    {
        "get_rtc",                      // command name (spaces are not allowed)
        "get current rtc time: yyyy-mm-dd-hh-mm-ss",   // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getRtcTimeCli                   // binding function
    },
    {
        "rtc_source",                   // command name (spaces are not allowed)
        "set rtc source: 1 - Internal, 2 - external, 3 - syncInternalWithExternal, 4 - DCF77",    // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        setRtcSourceCli                 // binding function
    },
    {
        "get_rtc_source",               // command name (spaces are not allowed)
        "get rtc source",               // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getRtcSourceCli                 // binding function
    },
    {
        "get_light_status",             // command name (spaces are not allowed)
        "get light status",             // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getLightStatusCli               // binding function
    }
}};


void sendCharOverSerial(EmbeddedCli *embeddedCli, char c) {
    Serial.write(c);
}

bool checkCStringIsNumber(const char *str) {
    for (size_t i = 0; i < strlen(str); ++i) {
        if (isdigit(str[i]) == 0) {
            return false;
        }
    }
    return true;
}

void saveValueToTimeVariable(ProjectTypes::RTC_Time& time_to_set, const uint32_t value, const size_t index) {
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
        if (!checkCStringIsNumber(token)) {
            Serial.println(F("Argument is not number!"));
            Serial.print(F("arg "));
            Serial.print((char) ('0' + i));
            Serial.print(F(": "));
            Serial.println(token);
            return;
        }
        // check if token is in range
        int32_t value = atoi(token);
        if (checkInputRtcArgumentValueIsInRange(value, i - 1) == false) {
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
    if (rtc_driver.setTimeToRtc(time_to_set))
    {
        Serial.println(F("New time is set!"));
        // update light state
        light_controller.periodicUpdateLightState();
    }
    else
    {
        Serial.println(F("Failed to set new time!"));
    }
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
    //  check only first token
    constexpr size_t kTokenToCheck = 1;
    const char *token = embeddedCliGetToken(args, kTokenToCheck);
    if (!checkCStringIsNumber(token)) {
        Serial.println(F("Argument is not number!"));
        Serial.print(F("arg : "));
        Serial.println(token);
        return;
    }
    // set new rtc source
    rtc_driver.setRtcSource((RtcSource) atoi(token));
}

void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    RtcSource source = rtc_driver.getRtcSource();
    switch (source)
    {
    case RtcSource::External:
        Serial.println("External rtc source");
        break;
    case RtcSource::Internal:
        Serial.println("Internal rtc source");
        break;
    case RtcSource::SyncExternalWithInternal:
        Serial.println("Sync internal with external rtc source");
        break;
    case RtcSource::DCF77:
        Serial.println("DCF77 rtc source");
        break;
    case RtcSource::None:
        Serial.println("Non selected rtc source");
        break;
    default:
        Serial.println("Error rtc source");
        break;
    }
}

void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    LightControl::LightState light_status = light_controller.getLightState();
    switch (light_status) {
    case LightControl::LightState::On:
        Serial.println("Light is on");
        break;
    case LightControl::LightState::Off:
        Serial.println("Light is off");
        break;
    case LightControl::LightState::Blanking:
        Serial.println("Light is blanking");
        break;
    case LightControl::LightState::Error:
        Serial.println("Light is in error state");
        break;
    case LightControl::LightState::Undefined:
    default:
        Serial.println("Light is in undefined state");
        break;
    }
}

/*
    @details check input value is in range
    index 0 - year
    index 1 - month
    index 2 - day
    index 3 - hour
    index 4 - minute
    index 5 - second
    @return true if value is in range, false otherwise
*/
bool checkInputRtcArgumentValueIsInRange(uint32_t value, size_t index) {
    bool status = false;    // default value
    switch (index) {
    case 0: // year
        if(value > 2000 && value < 2100)
            status = true;
        break;
    case 1: // month
        if(value > 0 && value < 13)
            status = true;
        break;
    case 2: // day
        if(value > 0 && value < 32)
            status = true;
        break;
    case 3: // hour
        if(value < 24)
            status = true;
        break;
    case 4: // minute
    case 5: // second
        if(value < 60)
            status = true;
        break;
    default:
        break;
    }

    return status;
}