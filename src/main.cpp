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
#include "rtc_driver.h"
#include "gpio_driver.h"
#include "cli_process.h"
#include "chicken_coop_controller.h"
#include "chicken_coop_controller_instance.h"

// Object of CLI interface controller
CLI::CLIProcess cli_process;

// Main chicken coop controller
ControlLogic::ChickenCoopController *chicken_coop_controller;

// Gpio driver
GPIO::GpioDriver *gpio_driver;
// Rtc driver
RtcDriver *rtc_driver;

ProjectTypes::time_ms_t light_process_last_time_ms = 0;
ProjectTypes::time_ms_t main_loop_loop_entry_time_ms = 0;
ProjectTypes::time_us_t main_loop_calculated_delay_us = 0;
ProjectTypes::time_us_t next_main_loop_process_time_us = 0;
ProjectTypes::time_us_t main_loop_current_time_us = 0;

// Setup program
void setup() {
    if (sizeof(std::time_t) == 4) {
        Serial.println("Time_t is 4 bytes, may overflow in 2038.");
    }
    //  Initialize serial
    Serial.begin(ProjectConst::kSerialBaudRate);
    // Initialize log
    initLog();
    LOG_INFO("Chicken coop controller started");
    // Initialize GPIO
    gpio_driver = GPIO::GpioDriver::getInstance();
    // Initialize RTC
    rtc_driver = &RtcDriver::getInstance();
    // Set rtc source to external module
    rtc_driver->setRtcSource(RtcSource::External);
    // Create chicken coop controller
    chicken_coop_controller = ControlLogic::getChickenCoopControllerInstance();
    delay(200);  //  delay for RTC to sync time

    // set first entry time for main loop
    main_loop_current_time_us = micros();
    LOG_INFO("Initialization done. Start main loop");
}

// Main program loop
void loop() {
    // get current time
    main_loop_loop_entry_time_ms = millis();
    // process light control every 1s
    if (main_loop_loop_entry_time_ms - light_process_last_time_ms >= ProjectConst::kMainLoopDelayBetweenLightControlProcess) {
        light_process_last_time_ms = main_loop_loop_entry_time_ms;
        chicken_coop_controller->periodicUpdateController();
    }

    // process CLI
    cli_process.periodicCProcessCLI();
    // calculate delay for main loop
    next_main_loop_process_time_us += ProjectConst::kMainLoopDelayUs;
    // get current time in us
    main_loop_current_time_us = micros();
    if (next_main_loop_process_time_us - main_loop_current_time_us > ProjectConst::kMainLoopDelayUs) {
        //  overrurn detected
        LOG_ERROR("Overrun main loop detected! Loop time: %d Current time: %d, next process time: %d",
                    next_main_loop_process_time_us - main_loop_current_time_us,
                    main_loop_current_time_us,
                    next_main_loop_process_time_us);
        // set new time for next main loop process
        next_main_loop_process_time_us = main_loop_current_time_us;
    } else {
        // delay for main loop
        delayMicroseconds(next_main_loop_process_time_us - main_loop_current_time_us);
    }
}
