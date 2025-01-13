/*
    Author: M.D
    Date: 2023/05/31
    Description: Chicken coop controller
*/

// Include library
#include <Arduino.h>
#include <climits>
#include <cmath>
#include <ctime>
#include "log.h"
#include "project_const.h"
#include "project_pin_definition.h"
#include "error_manager.h"
#include "diagnostic_manager.h"
#include "rtc_driver.h"
#include "gpio_driver.h"
#include "cli_process.h"
#include "chicken_coop_controller.h"
#include "chicken_coop_controller_instance.h"

// Object of CLI interface controller
CLI::CLIProcess cli_process;

// Main chicken coop controller
ControlLogic::ChickenCoopController *chicken_coop_controller;

// Error manager instance pointer
SystemControl::ErrorManager *error_manager;
// Diagnostic manager instance pointer
Diagnostic::DiagnosticManager *diagnostic_manager;
// Gpio driver
GPIOInterface::GpioDriver *gpio_driver;
// Rtc driver
RtcDriver *rtc_driver;

// Status of active light source
bool main_light_enabled = true;
bool external_light_enabled = true;

// Door status
auto door_control_mode = DoorControl::DoorControlMode::Off;
auto last_error_indicator_state = false;

// Time of last light process
ProjectTypes::time_ms_t light_process_last_time_ms = 0;
ProjectTypes::time_ms_t gpio_update_last_time_ms = 0;
ProjectTypes::time_ms_t main_loop_entry_time_ms = 0;
ProjectTypes::time_ms_t last_change_error_indicator = 0;
ProjectTypes::time_us_t next_main_loop_process_time_us = 0;
ProjectTypes::time_us_t main_loop_current_time_us = 0;

// Setup program
void setup() {
    //  Initialize serial
    Serial.begin(ProjectConst::kSerialBaudRate);
    if (sizeof(std::time_t) == 4) {
        Serial.println("Time_t is 4 bytes, may overflow in 2038.");
    }
    // Initialize log
    initLog();
    LOG_INFO("Chicken coop controller started");
    // Initialize error manager
    error_manager = SystemControl::ErrorManager::getInstance();
    // Initialize diagnostic manager
    diagnostic_manager = Diagnostic::DiagnosticManager::getInstance();
    // Initialize GPIO
    gpio_driver = GPIOInterface::GpioDriver::getInstance();
    // Initialize RTC
    rtc_driver = &RtcDriver::getInstance();
    // Set rtc source to external module
    rtc_driver->setRtcSource(RtcSource::External);
    // Create chicken coop controller
    chicken_coop_controller = ControlLogic::getChickenCoopControllerInstance();
    delay(200);  //  delay for RTC to sync with controller

    // set first entry time for main loop
    main_loop_current_time_us = micros();
    LOG_INFO("Initialization done. Start main loop");
}

// Main program loop
void loop() {
    // get current time
    main_loop_entry_time_ms = millis();

    // read control signals from external switch and buttons
    if (main_loop_entry_time_ms - gpio_update_last_time_ms >= ProjectConst::kMainLoopDelayBetweenUpdateGPIO) {
        gpio_update_last_time_ms = main_loop_entry_time_ms;
        // check if external light is enabled
        const auto external_light_source = gpio_driver->getExternalBuildingEnableControl();
        if (external_light_source != external_light_enabled) {
            external_light_enabled = external_light_source;
            chicken_coop_controller->toggleLightExternalBuilding(external_light_enabled);
        }
        // check if internal light is enabled
        const auto main_light_source = gpio_driver->getMainBuildingEnableControl();
        if (main_light_source != main_light_enabled) {
            main_light_enabled = main_light_source;
            chicken_coop_controller->toggleLightMainBuilding(main_light_source);
        }
        // check status of selected mode of main door
        const auto input_door_status = gpio_driver->getMainBuildingDoorControlMode();
        if (input_door_status != door_control_mode) {
            door_control_mode = input_door_status;
            // toggle main door controller based on selected mode
            chicken_coop_controller->toggleAutomaticDoorController(door_control_mode == DoorControl::DoorControlMode::Auto);
            LOG_INFO("Door status changed to %d", door_control_mode);
        }
        // take action when selected mode is manual
        if (door_control_mode == DoorControl::DoorControlMode::Manual) {
            // check if open signal is active
            if (gpio_driver->checkDoorControlOpenSignalIsActive()) {
                gpio_driver->setDoorControlAction(DoorControl::DoorControlAction::Open);
            } else if (gpio_driver->checkDoorControlCloseSignalIsActive()) {
                gpio_driver->setDoorControlAction(DoorControl::DoorControlAction::Close);
            } else {
                gpio_driver->setDoorControlAction(DoorControl::DoorControlAction::Disable);
            }
        }
    }

    // perform diagnostic
    diagnostic_manager->performDiagnostic();

    // process light control every 1s
    if (main_loop_entry_time_ms - light_process_last_time_ms >= ProjectConst::kMainLoopDelayBetweenLightControlProcess) {
        light_process_last_time_ms = main_loop_entry_time_ms;
        chicken_coop_controller->periodicUpdateController();
    }

    // show error indicator if error was occurred
    if (error_manager->checkIsAnyError()) {
        LOG_DEBUG("Error detected, count of errors: %d", error_manager->getErrorSet().size());
        if (error_manager->checkIsCriticalError()) {
            LOG_DEBUG("Critical error detected, light will be active whole time");
            gpio_driver->setErrorIndicator();
        } else {
            if (main_loop_entry_time_ms - last_change_error_indicator >= ProjectConst::kMainLoopDelayBetweenBlinkingErrorIndicator) {
                LOG_DEBUG("Normal error detected, state %d", static_cast<int>(last_error_indicator_state));
                last_change_error_indicator = main_loop_entry_time_ms;
                last_error_indicator_state = !last_error_indicator_state;
                gpio_driver->toggleErrorIndicator(last_error_indicator_state);
            }
        }
    } else {
        LOG_VERBOSE("No error detected");
        gpio_driver->clearErrorIndicator();
    }

    // process CLI
    cli_process.periodicCProcessCLI();

    // calculate delay for main loop
    next_main_loop_process_time_us += ProjectConst::kMainLoopDelayUs;
    // get current time in us
    main_loop_current_time_us = micros();
    if (next_main_loop_process_time_us - main_loop_current_time_us > ProjectConst::kMainLoopDelayUs) {
        //  overrurn detected
        LOG_ERROR("Overrun main loop detected! Loop time: %uus Current time: %uus, next process time: %uus",
                    main_loop_current_time_us - main_loop_entry_time_ms,
                    main_loop_current_time_us,
                    next_main_loop_process_time_us);
        // set new time for next main loop process
        next_main_loop_process_time_us = main_loop_current_time_us;
    } else {
        // delay for main loop
        delayMicroseconds(next_main_loop_process_time_us - main_loop_current_time_us);
    }
}
