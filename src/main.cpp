/*
    Author: M.D
    Date: 2023/05/31
    Description: This program is for automatic light control system.
    Support 2 modes:
        1. RTC clock to determine time of day
        2. Light sensor to determine light intensity
*/

// Include library
#include <Arduino.h>
#include <limits.h>
#include <math.h>
#include "project_const.h"
#include "project_pin_definition.h"
#include "light_controller_process.h"
#include "rtc_driver.h"
#include "gpio_driver.h"
#include "cli_process.h"

// Object of CLI interface controller
CLI::CLIProcess cli_process;

ProjectTypes::time_ms_t light_process_last_time_ms = 0;
ProjectTypes::time_ms_t main_loop_loop_entry_time_ms = 0;
ProjectTypes::time_us_t main_loop_calculated_delay_us = 0;
ProjectTypes::time_us_t next_main_loop_process_time_us = 0;
ProjectTypes::time_us_t main_loop_current_time_us = 0;

// Setup program
void setup()
{
    //Initialize serial
    Serial.begin(ProjectConst::kSerialBaudRate);

    // Initialize variable
    bool setup_ref_val_mode = GPIO::gpio_driver.getControlSelectSignal();

    // Set rtc source to external module
    rtc_driver.setRtcSource(RtcSource::External);
    delay(200); // delay for RTC to sync time

    // set first entry time for main loop
    main_loop_loop_entry_time_ms = micros();
}

// Main program loop
void loop()
{
    // get current time
    main_loop_loop_entry_time_ms = millis();
    // process light control every 1s
    if ( main_loop_loop_entry_time_ms - light_process_last_time_ms >= ProjectConst::kMainLoopDelayBetweenLightControlProcess) {
        light_process_last_time_ms = main_loop_loop_entry_time_ms;
        light_controller.periodicUpdateLightState();
    }

    // process CLI
    cli_process.periodicCProcessCLI();

    // calculate delay for main loop
    next_main_loop_process_time_us += ProjectConst::kMainLoopDelayUs;
    // get current time in us
    main_loop_current_time_us = micros();
    if (next_main_loop_process_time_us - main_loop_current_time_us > ProjectConst::kMainLoopDelayUs) {
        //overrurn detected
        Serial.println("Overrun main loop detected!!!");
        // set new time for next main loop process
        next_main_loop_process_time_us = main_loop_current_time_us;
    }
    else {
        // delay for main loop
        delayMicroseconds(next_main_loop_process_time_us - main_loop_current_time_us);
    }
}
