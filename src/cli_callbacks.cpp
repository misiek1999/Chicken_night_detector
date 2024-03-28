#include "cli_callbacks.h"

#include <Arduino.h>
#include <cstdlib>
#include <ctime>
#include "light_state.h"
#include "chicken_coop_controller.h"
#include "chicken_coop_controller_impl.h"
#include "rtc_driver.h"

// declaration of helper function
bool checkInputRtcArgumentValueIsInRange(uint32_t value, size_t index);

// declaration of function to process command
void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);

CLI::CliCommandContainer CLI::cli_callbacks = {{{
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

void saveValueToTimeVariable(std::tm& time_to_set, const int value, const size_t index) {
    switch (index) {
        case 0:
            time_to_set.tm_year = value;
            break;
        case 1:
            time_to_set.tm_mon = value;
            break;
        case 2:
            time_to_set.tm_mday = value;
            break;
        case 3:
            time_to_set.tm_hour = value;
            break;
        case 4:
            time_to_set.tm_min = value;
            break;
        case 5:
            time_to_set.tm_sec = value;
            break;
        default:
            break;
    }
}

void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    auto* rtc_driver_ptr = &RtcDriver::getInstance();
    // Time value
    std::tm time_to_set = {};
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
    // convert tm to time_t
    const auto time_to_set_time_t = std::mktime(&time_to_set);
    // set rtc time
    if (rtc_driver_ptr->setTimeToRtc(time_to_set_time_t)) {
        Serial.println(F("New time is set!"));
        // update light state
        chicken_coop_controller_ptr->periodicUpdateController();
    } else {
        Serial.println(F("Failed to set new time!"));
    }
}

void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    auto* rtc_driver_ptr = &RtcDriver::getInstance();
    // read current time
    std::time_t currentTime = rtc_driver_ptr->getCurrentTimeRtc();
    // convert time_t to tm
    auto currentTimeTm = *std::localtime(&currentTime);
    // print current time
    char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "Time: %04d-%02d-%02d %02d:%02d:%02d",
        currentTimeTm.tm_year, currentTimeTm.tm_mon, currentTimeTm.tm_mday,
        currentTimeTm.tm_hour, currentTimeTm.tm_min, currentTimeTm.tm_sec);
    // Print the formatted string to serial so we can see the time.
    Serial.println(buf);
    const RtcStatus rtc_status = rtc_driver_ptr->getRtcStatus();
    if (rtc_status == RtcStatus::Disconnected)
        Serial.println("Sensor is disconnected!");
    else if (rtc_status == RtcStatus::Uninitialized)
        Serial.println("Sensor is uninitialized!");
}

void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    auto* rtc_driver_ptr = &RtcDriver::getInstance();
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
    rtc_driver_ptr->setRtcSource((RtcSource) atoi(token));
}

void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    auto* rtc_driver_ptr = &RtcDriver::getInstance();
    RtcSource source = rtc_driver_ptr->getRtcSource();
    switch (source) {
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
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    auto light_status = chicken_coop_controller_ptr->getLightStates();
    for (auto light_status_it : light_status) {
        // TODO: Add ID translation to string
        Serial.print("Building ID: ");
        Serial.print((int)light_status_it.first);
         Serial.print(" -> ");
        switch (light_status_it.second) {
        case ControlLogic::LightState::On:
            Serial.println("Light is on");
            break;
        case ControlLogic::LightState::Off:
            Serial.println("Light is off");
            break;
        case ControlLogic::LightState::Dimming:
            Serial.println("Light is dimming");
            break;
        case ControlLogic::LightState::Error:
            Serial.println("Light is in error state");
            break;
        case ControlLogic::LightState::Undefined:
        default:
            Serial.println("Light is in undefined state");
            break;
        }
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