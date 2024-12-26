#include "cli_callbacks.h"

#include <Arduino.h>
#include <cstdlib>
#include <ctime>
#include "light_state.h"
#include "chicken_coop_controller.h"
#include "chicken_coop_controller_instance.h"
#include "rtc_driver.h"
#include "log.h"

// declaration of helper function
bool checkInputRtcArgumentValueIsInRange(uint32_t value, size_t index);

// declaration of function to process command
void setRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcTimeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getLightStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);
void setExternalLightCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getExternalLightCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getDoorStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);
void changeLogLevelCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getErrorStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);
void getDoorControlModeCli(EmbeddedCli *embeddedCli, char *args, void *context);
void clearAllErrorStatusCli(EmbeddedCli *embeddedCli, char *args, void *context);

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
    },
    {
        "toggle_external_light",           // command name (spaces are not allowed)
        "toggle external light: 0 - off, 1 -on",  // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        setExternalLightCli             // binding function
    },
    {
        "get_external_light",           // command name (spaces are not allowed)
        "get external light",           // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getExternalLightCli             // binding function
    },
    {
        "get_door_status",              // command name (spaces are not allowed)
        "get door status",              // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getDoorStatusCli                // binding function
    },
    {
        "change_log_level",             // command name (spaces are not allowed)
        "change log level: 0 - off, 1 - fatal, 2 - error, 3 - warning, 4 - info, 5 - debug, 6 - verbose",  // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        changeLogLevelCli               // binding function
    },
    {
        "get_error_status",             // command name (spaces are not allowed)
        "get error status",             // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getErrorStatusCli               // binding function
    },
    {
        "get_door_control_mode",        // command name (spaces are not allowed)
        "get door control mode",        // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        getDoorControlModeCli           // binding function
    },
    {
        "clear_error_status",           // command name (spaces are not allowed)
        "clear error status",           // Optional help for a command (NULL for no help)
        false,                          // flag whether to tokenize arguments (see below)
        nullptr,                        // optional pointer to any application context
        clearAllErrorStatusCli          // binding function
    }
}};


void sendCharOverSerial(EmbeddedCli *embeddedCli, char c) {
    (void)embeddedCli;
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
            time_to_set.tm_year = value - ProjectConst::kTmStructInitYear;
            break;
        case 1:
            time_to_set.tm_mon = value - ProjectConst::kMonthSyncOffset;
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
    (void)embeddedCli;
    (void)context;
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
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* rtc_driver_ptr = &RtcDriver::getInstance();
    // read current time
    std::time_t currentTime = rtc_driver_ptr->getTimeFromRtc();
    // convert time_t to tm
    auto currentTimeTm = *std::localtime(&currentTime);
    // print current time
    char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "Time: %04d-%02d-%02d %02d:%02d:%02d",
        currentTimeTm.tm_year + ProjectConst::kTmStructInitYear, currentTimeTm.tm_mon + ProjectConst::kMonthSyncOffset,
        currentTimeTm.tm_mday, currentTimeTm.tm_hour, currentTimeTm.tm_min, currentTimeTm.tm_sec);
    // Print the formatted string to serial so we can see the time.
    Serial.println(buf);
    const RtcStatus rtc_status = rtc_driver_ptr->checkRtcIsOk();
    if (rtc_status == RtcStatus::Disconnected)
        Serial.println("Sensor is disconnected!");
    else if (rtc_status == RtcStatus::Uninitialized)
        Serial.println("Sensor is uninitialized!");
}

void setRtcSourceCli(EmbeddedCli *embeddedCli, char *args, void *context) {
    (void)embeddedCli;
    (void)context;
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
    (void)embeddedCli;
    (void)context;
    (void)args;
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
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    auto light_status = chicken_coop_controller_ptr->getLightStates();
    for (auto& [building_id, light_state] : light_status) {
        // TODO: Add ID translation to string
        const auto  rest_of_time = chicken_coop_controller_ptr->getDimmingTime(static_cast<ControlLogic::BuildingId>(building_id));
        Serial.print("Building ID: ");
        Serial.print(static_cast<int>(building_id));
        Serial.print(" -> ");
        switch (light_state) {
        case ControlLogic::LightState::On:
            Serial.println("Light is on");
            break;
        case ControlLogic::LightState::Off:
            Serial.println("Light is off");
            break;
        case ControlLogic::LightState::Dimming:
            Serial.print("Light is dimming. Rest of time: ");
            Serial.println(rest_of_time);
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

void setExternalLightCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    //  check only first token
    constexpr size_t kTokenToCheck = 1;
    const char *token = embeddedCliGetToken(args, kTokenToCheck);
    if (!checkCStringIsNumber(token)) {
        Serial.println(F("Argument is not number!"));
        Serial.print(F("arg : "));
        Serial.println(token);
        return;
    }
    // read state from token
    const auto state = static_cast<bool>(atoi(token));
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    chicken_coop_controller_ptr->toggleLightExternalBuilding(state);
}

void getExternalLightCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    const bool state = chicken_coop_controller_ptr->checkLightControllerInExternalBuildingIsActive();
    Serial.print("External light state: ");
    Serial.println(state);
}

void getDoorStatusCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* chicken_coop_controller_ptr = ControlLogic::getChickenCoopControllerInstance();
    auto door_status = chicken_coop_controller_ptr->getDoorActions();
    for (auto door_status_it : door_status) {
        Serial.print("Building ID: ");
        Serial.print(static_cast<int>(door_status_it.first));
        Serial.print(" -> ");
        switch (door_status_it.second) {
        case DoorControl::DoorControlAction::Open:
            Serial.println("Door is open");
            break;
        case DoorControl::DoorControlAction::Close:
            Serial.println("Door is close");
            break;
        case DoorControl::DoorControlAction::Disable:
            Serial.println("Door is disabled");
            break;
        default:
            Serial.println("Door is in error state");
            break;
        }
        Serial.println("");
    }
}

void changeLogLevelCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    //  check only first token
    constexpr size_t kTokenToCheck = 1;
    const char *token = embeddedCliGetToken(args, kTokenToCheck);
    if (!checkCStringIsNumber(token)) {
        Serial.println(F("Argument is not number!"));
        Serial.print(F("arg : "));
        Serial.println(token);
        return;
    }
    // set new log level
    const auto log_level = atoi(token);
    changeLogLevel(log_level);
    Serial.print("New log level: ");
    Serial.println(token);
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
    case 0:  // year
        if (value > 2000 && value < 2100)
            status = true;
        break;
    case 1:  // month
        if (value > 0 && value < 13)
            status = true;
        break;
    case 2:  // day
        if (value > 0 && value < 32)
            status = true;
        break;
    case 3:  // hour
        if (value < 24)
            status = true;
        break;
    case 4:  // minute
    case 5:  // second
        if (value < 60)
            status = true;
        break;
    default:
        break;
    }

    return status;
}

void getErrorStatusCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* error_manager_ptr = SystemControl::ErrorManager::getInstance();
    const bool error_status = error_manager_ptr->checkIsAnyError();
    if (error_status) {
        Serial.print("Error: ");
        for (const auto& error_code : error_manager_ptr->getErrorSet()) {
            Serial.print(SystemControl::getErrorCodeName(error_code));
            Serial.print(", ");
        }
        Serial.println("");
    } else {
        Serial.println("No errors");
    }
}

void getDoorControlModeCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* gpio_driver_ptr = GPIOInterface::GpioDriver::getInstance();
    const auto door_control_mode = gpio_driver_ptr->getMainBuildingDoorControlMode();
    Serial.print("Door control mode: ");
    Serial.println(static_cast<int>(door_control_mode));
}

void clearAllErrorStatusCli(EmbeddedCli * embeddedCli, char * args, void * context) {
    (void)embeddedCli;
    (void)context;
    (void)args;
    auto* error_manager_ptr = SystemControl::ErrorManager::getInstance();
    error_manager_ptr->resetAllError();
    Serial.println("All errors are cleared");
}
