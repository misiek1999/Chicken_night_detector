#pragma once

#include "light_state.h"
#include "light_event_manager.h"

namespace ControlLogic {

class IBulbController {
public:
    virtual ~IBulbController() = default;

    /*
        @brief: update bulb controller
        @return: true if success, false if failed
    */
    virtual bool periodicUpdateController() = 0;

    /*
        @brief: get light state
        @return: light state
    */
    virtual ControlLogic::LightState getLightState() = 0;

    /*
        @brief: get total of dimming time
        @param: current_time - current time
        @return: total of dimming time
    */
    virtual float getTotalOfDimmingTimePercent() const = 0;
};
}  // namespace ControlLogic
