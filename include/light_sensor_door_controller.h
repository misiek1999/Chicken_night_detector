#pragma once

#include <external_light_sensor_callback.h>
#include "project_types.h"
#include "gpio_driver.h"
#include "door_controller.h"

namespace ControlLogic {

    class LightSensorDoorController : public IDoorController {
     public:
        explicit LightSensorDoorController(lightStateCallback light_sensor_callback);


        /*
            @brief: Update door controller, should be called periodically
            @return: true if success, false if failed
        */
        bool updateDoorControllerEvents() override;

        /*
            @brief: Get door state
            @return: door action state
        */
        DoorControl::DoorControlAction getDoorState() const override;

     private:
        lightStateCallback light_sensor_callback_;
        ProjectTypes::time_ms_t last_light_change_time_ {0};
        EnternalLightState last_light_state_ {EnternalLightState::Low};
        DoorControl::DoorControlAction door_state_ {DoorControl::DoorControlAction::Disable};
    };


}  // namespace ControlLogic
