#include "chicken_coop_controller_impl.h"

static ControlLogic::CoopConfig getCoopConfig() {
        static auto coop_config = ControlLogic::CoopConfig{
            .light_state_config_ ={
            ControlLogic::LightStateConfig{
                .callback_ = {
                    .toogle_light_state = [](const bool &state) {
                        GPIO::GpioDriver::getInstance()->toggleLightMainBuilding(state);
                    },
                    .set_pwm_light_percentage = [](const float &percent_light) {
                        GPIO::GpioDriver::getInstance()->setPWMLightPercentageMainBuilding(percent_light);
                    }
                },
                .id_ = ControlLogic::BuildingId::Main,
                .is_active_ = true
            },
            {
                .callback_ = {
                    .toogle_light_state = [](const bool &state) {
                        GPIO::GpioDriver::getInstance()->toggleLightExternalBuilding(state);
                    },
                    .set_pwm_light_percentage = [](const float &percent_light) {
                        GPIO::GpioDriver::getInstance()->setPWMLightPercentageExternalBuilding(percent_light);
                    }
                },
                .id_ = ControlLogic::BuildingId::External,
                .is_active_ = true
            }
        },
        .door_config_ = {
            ControlLogic::DoorStateConfig{
                .callback_ = {
                    .toogle_door_state = [](const DoorControl::DoorControlAction &state) {
                        GPIO::GpioDriver::getInstance()->setDoorControlAction(state);
                    }
                },
                .id_ = ControlLogic::BuildingId::Main,
                .is_active_ = true
            }
        }
    };
    return coop_config;
}

ControlLogic::ChickenCoopController* ControlLogic::getChickenCoopControllerInstance() {
    static auto getRtcTime = []() -> std::time_t {
        return RtcDriver::getInstance().getCurrentTimeRtc();
    };
    static ControlLogic::ChickenCoopController chicken_coop_controller_instance(getCoopConfig(), getRtcTime);
    return &chicken_coop_controller_instance;
}
