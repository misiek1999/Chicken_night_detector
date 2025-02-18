#pragma once

#include "gpio_driver.h"
#include "light_sensor_door_controller.h"
#include "light_sensor_bulb_controller.h"

namespace ControlLogic {
    LightSensorDoorController* getLightSensorDoorControllerInstance() {
        static LightSensorDoorController light_sensor_door_controller([]() {
            return GPIOInterface::GpioDriver::getInstance()->getExternalLightSensor();
        });
        return &light_sensor_door_controller;
    }

    LightSensorBulbController* getLightSensorBulbControllerInstance() {
        static LightSensorBulbController light_sensor_bulb_controller([]() {
            return GPIOInterface::GpioDriver::getInstance()->getExternalLightSensor();
        });
        return &light_sensor_bulb_controller;
    }
}  // namespace ControlLogic
