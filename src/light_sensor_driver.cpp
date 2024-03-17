#include "light_sensor_driver.h"
#include "gpio_driver.h"


LightSensorDriver::LightSensorDriver() {
}

LightControl::LightState LightSensorDriver::getLightState(const ProjectTypes::RTC_Time & time_now) {
    //TODO: Implement this method
    return LightControl::LightState::Off;
}

ProjectTypes::analog_signal_t LightSensorDriver::readAnalogSignalFromSensor() {
    return GPIO::gpio_driver.getLightAnalogSensorValue();
}