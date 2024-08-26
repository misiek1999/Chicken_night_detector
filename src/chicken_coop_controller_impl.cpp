#include "chicken_coop_controller_instance.h"

static ControlLogic::CoopConfig getCoopConfig() {
        static auto coop_config = ControlLogic::CoopConfig{
            .light_state_config_ ={
            ControlLogic::LightStateConfig{
                .callback_ = {
                    .toogle_light_state = [](const bool &state) {
                        GPIOInterface::GpioDriver::getInstance()->toggleLightMainBuilding(state);
                    },
                    .set_pwm_light_percentage = [](const float &percent_light) {
                        GPIOInterface::GpioDriver::getInstance()->setPWMLightPercentageMainBuilding(percent_light);
                    }
                },
                .id_ = ControlLogic::BuildingId::Main,
                .is_active_ = true
            },
            {
                .callback_ = {
                    .toogle_light_state = [](const bool &state) {
                        GPIOInterface::GpioDriver::getInstance()->toggleLightExternalBuilding(state);
                    },
                    .set_pwm_light_percentage = [](const float &percent_light) {
                        GPIOInterface::GpioDriver::getInstance()->setPWMLightPercentageExternalBuilding(percent_light);
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
                        GPIOInterface::GpioDriver::getInstance()->setDoorControlAction(state);
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
    static auto getRtcTime = []() {
        return RtcDriver::getInstance().getCurrentTimeRtc();
    };
    static ControlLogic::ChickenCoopController chicken_coop_controller_instance(getCoopConfig(), getRtcTime);
    return &chicken_coop_controller_instance;
}
