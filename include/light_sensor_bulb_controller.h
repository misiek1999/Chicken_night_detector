#pragma once

#include "project_types.h"
#include "external_light_sensor_callback.h"
#include "light_state.h"
#include "I_bulb_controller.h"
#include "gpio_driver.h"

namespace ControlLogic {
class LightSensorBulbController : public IBulbController {
public:
    explicit LightSensorBulbController(lightStateCallback light_sensor_callback);
    bool periodicUpdateController() override;

    LightState getLightState() override;

    /*
        @brief: This function is not supported by this controller
        @return: 0
    */
    float getTotalOfDimmingTimePercent() const override {
        return 0.0F;
    }

private:
    constexpr static auto kDelayToDisableLightMs = 1800000;    // 30 minutes
    lightStateCallback light_sensor_callback_;
    ProjectTypes::time_ms_t last_light_change_time_ {0};
    LightState light_state_ {LightState::Off};
};

}  // namespace ControlLogic
