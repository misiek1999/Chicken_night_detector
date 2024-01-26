#include "gpio_driver.h"
#include "project_types.h"
#include "project_pin_definition.h"

// Global object of GpioDriver
GPIO::GpioDriver GPIO::gpio_driver;

GPIO::GpioDriver::GpioDriver()
{
    // initialize all gpio
    pinMode(PHOTORESISTOR_ADC_PIN, INPUT_ANALOG);
    pinMode(REF_ADC_PIN, INPUT_ANALOG);
    pinMode(ON_BOARD_LED_PIN, OUTPUT);
    pinMode(SELECT_MODE_PIN, INPUT_PULLDOWN);
    // lights are off by default
    digitalWrite(LIGHT_OUT_PIN, LOW);
    digitalWrite(ON_BOARD_LED_PIN, HIGH);
}

void GPIO::GpioDriver::toggleLight(const bool state)
{
    if(state)
    {
        pinMode(LIGHT_OUT_PIN, OUTPUT);
        digitalWrite(LIGHT_OUT_PIN, HIGH);
        digitalWrite(ON_BOARD_LED_PIN, LOW);
    }
    else
    {
        pinMode(LIGHT_OUT_PIN, OUTPUT);
        digitalWrite(LIGHT_OUT_PIN, LOW);
        digitalWrite(ON_BOARD_LED_PIN, HIGH);
    }

}

constexpr float kLightPercentOff = 0.5;
constexpr float kLightPercentOn = 1.0;
void GPIO::GpioDriver::setPWMLightPercentage(const float percent_light)
{
    // scale percent range
    float scaled_percent_light = percent_light * (kLightPercentOn - kLightPercentOff) + kLightPercentOff;
    // convert percent light to the range of 0 to 1023
    const uint16_t pwm = static_cast<uint16_t>(scaled_percent_light * 1023.0);
    setPWMLight(pwm);
}

void GPIO::GpioDriver::setPWMLight(const uint16_t pwm)
{
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    analogWrite(LIGHT_OUT_PIN, pwm);
}

// TODO: implement this function
bool GPIO::GpioDriver::getControlSelectSignal()
{
    return digitalRead(SELECT_MODE_PIN);
}

ProjectTypes::analog_signal_t GPIO::GpioDriver::getLightAnalogSensorValue()
{
    return analogRead(PHOTORESISTOR_ADC_PIN);
}

ProjectTypes::analog_signal_t GPIO::GpioDriver::getControlAnalogSensorValue()
{
    return analogRead(REF_ADC_PIN);
}
