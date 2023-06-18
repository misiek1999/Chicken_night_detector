/*
    Author: M.D
    Date: 2023/05/31
    Description: This program is for automatic light control system.
    Support 2 modes:
        1. RTC clock to determine time of day
        2. Light sensor to determine light intensity
*/

// macro to disable HAL library
// #define HAL_SPI_MODULE_DISABLED
// #define HAL_I2C_MODULE_DISABLED
// TODO: check this method

// Include library
#include <Arduino.h>
#include <limits.h>
#include <math.h>

#include <IWatchdog.h>

#include "project_const.h"
#include "project_pin_definition.h"
#include "light_control.h"
#include "rtc_controler.h"
#include "cli_interface.h"
#include "calculate_night_time.h"

enum class ProgramMode
{
    RTC,            // Recommenced mode
    LightSensor,    // Unrecommended mode in dark place
    None
};
ProgramMode program_state = ProgramMode::RTC; // Default state

static Time time_curr(0, 0, 0, 0, 0, 0, (Time::Day)0);

// Local variable
uint16_t photoresistor_adc_val;
uint16_t reference_adc_val;
float median_photoresistor_adc_val;
float median_reference_adc_val;

bool setup_ref_val_mode;
bool is_dark;

uint32_t mainLoopCounterLightProcess = 0;
uint32_t delayMainLoopUs = 0;
uint32_t loopEntryTimeUs = 0;

// Local functions declaration
bool compare_values();
void periodicProcessLightControl();

// Setup program
void setup()
{
    //Initialize serial
    Serial.begin(115200);

    //Initialize GPIO
    pinMode(PHOTORESISTOR_ADC_PIN, INPUT_ANALOG);
    pinMode(REF_ADC_PIN, INPUT_ANALOG);
    pinMode(ON_BOARD_LED_PIN, OUTPUT);
    pinMode(SELECT_MODE_PIN, INPUT_PULLDOWN);
    digitalWrite(LIGHT_OUT_PIN, LOW);   //Default lights are off
    digitalWrite(ON_BOARD_LED_PIN, HIGH);

    // Initialize variable
    setup_ref_val_mode = false;

    // Initialize light control
    initLightControl();

    // Initialize RTC
    initRtc();
    setRtcSource(RtcSource::External);
    delay(200); // delay for RTC to sync time
    // read time from RTC and check if it is valid
    if  (getRtcStatus() != RtcStatus::Ok) { // if RTC is not valid
        // change program mode to light sensor
        program_state = ProgramMode::LightSensor;
        Serial.println("RTC is not valid, change to light sensor mode");
    } else {
        // change program mode to RTC
        program_state = ProgramMode::RTC;
        Serial.println("RTC is valid, change to RTC mode");
    }

    // Initialize CLI
    CLI::init_CLI();
}

// Main program loop
void loop()
{
    loopEntryTimeUs = micros();
    // process light control every 1s
    if (mainLoopCounterLightProcess++ >= ProjectConst::kMainLoopCountToProcessLightDetect) {
        mainLoopCounterLightProcess = 0;
        periodicProcessLightControl();
    }

    // process CLI
    CLI::periodicCProcessCLI();

    // calculate delay for main loop
    delayMainLoopUs = (loopEntryTimeUs + ProjectConst::kMainLoopDelayUs) - micros();
    // check calculated delay is correct
    if (delayMainLoopUs < ProjectConst::kMainLoopDelayUs) {
        delay_us(delayMainLoopUs);  //TODO: change this function to more accurate
    }
    else { //this case mean probably overrun
        // TODO: add overrun support
    }
}

// Return true when reference voltage is lower than photoresistor voltage
bool compare_values() {
    median_photoresistor_adc_val = 0;
    median_reference_adc_val = 0;
    for (size_t i =0; i < ProjectConst::kPhotoresistorSensorCount; ++i) {
        median_photoresistor_adc_val += 1.0f/ProjectConst::kPhotoresistorSensorCount * analogRead(PHOTORESISTOR_ADC_PIN);
        median_reference_adc_val += 1.0f/ProjectConst::kPhotoresistorSensorCount * analogRead(REF_ADC_PIN);
    }
    if (median_photoresistor_adc_val < median_reference_adc_val )
        return true;
    else
        return false;
}

void periodicProcessLightControl() {
    // Read select pin mode
    setup_ref_val_mode = ! digitalRead(SELECT_MODE_PIN);
    // Select mode of detection dark
    switch (program_state)
    {
    case ProgramMode::RTC:          // Detect night using RTC
        is_dark = detectNightUsingRtc();
        break;
    case ProgramMode::LightSensor:  // Detect night using photoresistor
        is_dark = compare_values();
        break;
    case ProgramMode::None:         // Do nothing
        break;
    default:
        break;
    }
    // Update light control
    periodicUpdateLightControl(is_dark);
}
