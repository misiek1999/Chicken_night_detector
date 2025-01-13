#pragma once

#include "light_state.h"
#include "light_event_manager.h"
#include "I_bulb_controller.h"
#include "calculate_sunset_and_sunrise_time.h"
#include "rtc_driver.h"
#include "light_event_duration.h"

namespace ControlLogic {
class RtcBulbController : public IBulbController {
public:
    RtcBulbController() = delete;
    explicit RtcBulbController(TimeCallback* rtc_update_time_callback, DaytimeCalculator* daytime_calculator);

    bool periodicUpdateController() override;

    LightState getLightState() override;

    float getTotalOfDimmingTimePercent() const override {
        return light_event_manager_.getTotalOfDimmingTimePercent(last_update_time_);
    }

    // Methods from LightEventManager
    auto getEventState(const std::time_t &current_time, const size_t event_index) {
        return light_event_manager_.getEventState(current_time, event_index);
    }
    auto updateAllDimmingTime(const ProjectTypes::time_minute_t &new_dimming_time) {
        light_event_manager_.updateAllDimmingTime(new_dimming_time);
    }
    auto updateAllActivationAndDimmingTime(const ProjectTypes::time_minute_t &new_activation_time,
                                           const ProjectTypes::time_minute_t &new_deactivation_time) {
        light_event_manager_.updateAllActivationAndDeactivationTime(new_activation_time, new_deactivation_time);
    }
    auto getTotalOfDimmingTime(const std::time_t &current_time) const {
        return light_event_manager_.getTotalOfDimmingTime(current_time);
    }
private:
    constexpr static auto kDefaultEventIndex = 0U;
    TimeCallback* rtc_update_time_callback_;
    DaytimeCalculator* daytime_calculator_;
    LightEventManager light_event_manager_;
    std::time_t last_update_time_ = {};
};

}  // namespace ControlLogic
