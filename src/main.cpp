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
#include <libmaple/iwdg.h>
#include "project_const.h"
#include "project_pin_definition.h"
#include "light_control.h"
#include "rtc_driver.h"
#include "gpio_driver.h"
#include "cli_process.h"

// Object of light controller
LightControl::LightController light_controller;
// Object of CLI interface controller
CLI::CLIProcess cli_process;

//TODO: remove below local variables from main.cpp
// Local variable
uint16_t photoresistor_adc_val;
uint16_t reference_adc_val;
float median_photoresistor_adc_val;
float median_reference_adc_val;

bool setup_ref_val_mode;

uint32_t mainLoopCounterLightProcess = 0;
uint32_t delayMainLoopUs = 0;
uint32_t loopEntryTimeUs = 0;

// Setup program
void setup()
{
    //Initialize serial
    Serial.begin(ProjectConst::kSerialBaudRate);

    // Initialize variable
    setup_ref_val_mode = GPIO::gpio_driver.getControlSelectSignal();
    delay(200); // delay for RTC to sync time

    // Initialize CLI
    // CLI::init_CLI();
}

// Main program loop
void loop()
{
    loopEntryTimeUs = micros();
    // process light control every 1s
    if (mainLoopCounterLightProcess++ >= ProjectConst::kMainLoopCountToProcessLightDetect) {
        mainLoopCounterLightProcess = 0;
        light_controller.periodicUpdateLightState();
    }

    // process CLI
    cli_process.periodicCProcessCLI();

    // calculate delay for main loop
    delayMainLoopUs = (loopEntryTimeUs + ProjectConst::kMainLoopDelayUs) - micros();
    // check calculated delay is correct
    if (delayMainLoopUs < ProjectConst::kMainLoopDelayUs) {
        delay_us(delayMainLoopUs);  //TODO: change this function to more accurate
    }
    else { 
        // TODO: add overrun support
    }
}
