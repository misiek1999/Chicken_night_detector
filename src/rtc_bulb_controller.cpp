#include "rtc_bulb_controller.h"
#include "project_const.h"
#include "log.h"

namespace ControlLogic {

EventUpdateCallback sunset_callback_;

RtcBulbController::RtcBulbController(TimeCallback* rtc_update_time_callback,
                                     DaytimeCalculator* daytime_calculator) :
        rtc_update_time_callback_(rtc_update_time_callback),
        daytime_calculator_(daytime_calculator)
{
    assert(rtc_update_time_callback_ != nullptr && *rtc_update_time_callback_ && "RTC time callback is nullptr!");
    last_update_time_ = std::invoke(*rtc_update_time_callback_);

    auto event_time = daytime_calculator_->getSunsetTime(last_update_time_);
    const auto date = *std::localtime(&last_update_time_);
    const auto month = static_cast<uint8_t>(date.tm_mon);
    const auto [time_to_blink_before_event, time_to_blink_after_event, time_to_turn_on_before_event, time_to_turn_off_after_event] = ControlLogic::getEventDurationTime(month);

    sunset_callback_ = [daytime_calculator](const std::time_t &current_time) -> std::time_t {
        const auto data = daytime_calculator->getSunsetTime(current_time);
        return data;
    };

    light_event_manager_ = std::move(LightEventManager(
        LightDimmingEventMap { etl::pair {
            kDefaultEventIndex,
            LightDimmingEventAndCallback {
                .event =
                    LightDimmingEvent(event_time,
                        0,
                        time_to_turn_off_after_event,
                        time_to_blink_before_event,
                        time_to_blink_after_event),
                .callback = std::move(sunset_callback_)
            }
        }}
    ));
}

bool RtcBulbController::periodicUpdateController() {
    last_update_time_ = (*rtc_update_time_callback_)();
    // update dimming and normal light duration
    const auto month = (*std::localtime(&last_update_time_)).tm_mon;
    const auto [time_to_blink_before_event, time_to_blink_after_event, time_to_turn_on_before_event, time_to_turn_off_after_event] = ControlLogic::getEventDurationTime(month);
    light_event_manager_.updateEventDimmingTime(time_to_blink_after_event, kDefaultEventIndex);
    light_event_manager_.updateActivationAndDeactivationTime(time_to_turn_on_before_event, time_to_turn_off_after_event, kDefaultEventIndex);
    LOG_VERBOSE("Update dimming time %d", time_to_blink_after_event);
    LOG_VERBOSE("Update activation time Bef: %d Aft: %d", time_to_turn_on_before_event, time_to_turn_off_after_event);
    light_event_manager_.updateEvents(last_update_time_);
    return true;
}

LightState RtcBulbController::getLightState() {
    return light_event_manager_.getLightState(last_update_time_);
}

}  // namespace ControlLogic
