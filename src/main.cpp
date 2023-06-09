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
#include "light_control.h"
#include "rtc_controler.h"
#include "cli_interface.h"
#include "calculate_night_time.h"

// Const value
const uint32_t COUNT_OF_SAMPLES = 5;

enum class ProgramMode
{
    RTC,            // Recomended mode
    LightSensor,    // Unrecomended mode in dark place
    None
};
ProgramMode program_state = ProgramMode::RTC; // Default state

Time time_curr(2013, 9, 22, 1, 38, 50, Time::Day::kFriday);

// Local variable
uint16_t photoresistor_adc_val;
uint16_t reference_adc_val;
float median_photoresistor_adc_val;
float median_reference_adc_val;

bool setup_ref_val_mode;
bool is_dark;

// Local dunctions declaration
bool compare_values();
Time calculateSunsetTime(const Time current_time);

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

    // setTimeToRtc(time_curr);   // TODO: delete this function after debug test
}

// Main program loop
void loop()
{
    // Read select pin mode
    setup_ref_val_mode = ! digitalRead(SELECT_MODE_PIN);
    // Selcect mode of detection dark
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
    perdiocUpdateLightControl(is_dark);

    char buf[50];
    // Get the time from the RTC
    snprintf(buf, sizeof(buf), "test: %04d-%02d-%02d %02d:%02d:%02d",
        time_curr.yr, time_curr.mon, time_curr.date,
        time_curr.hr, time_curr.min, time_curr.sec);

    // Print the formatted string to serial so we can see the time.
     Serial.println(buf);
    // Delay for 1s
    getCurrentTimeRtc(time_curr);
    delay(1000);
}

// Return true when reference voltage is lower than photoresistor voltage
bool compare_values() {
    median_photoresistor_adc_val = 0;
    median_reference_adc_val = 0;
    for (int i =0; i < COUNT_OF_SAMPLES; ++i) {
        median_photoresistor_adc_val += 1.0f/COUNT_OF_SAMPLES * analogRead(PHOTORESISTOR_ADC_PIN);
        median_reference_adc_val += 1.0f/COUNT_OF_SAMPLES * analogRead(REF_ADC_PIN);
    }
    if (median_photoresistor_adc_val < median_reference_adc_val )
        return true;
    else
        return false;
}

Time calculateSunsetTime(const Time current_time) {
    Time sunset_time = current_time;
    float year =  current_time.yr;
    float month = current_time.mon;
    float day = current_time.day;
    // equation of sunset time


    return sunset_time;
}
