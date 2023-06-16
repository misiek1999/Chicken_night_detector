#include "light_control.h"

static LightState light_state = LightState::Off; // Default state
static uint32_t second_count_to_change_light_state = 0;   // Count of seconds to change light state
static uint32_t count_of_second_in_night = 0;   // Count of seconds when light signal is on
/*
    @details change light state to turn off
*/
void turn_off_light();

/*
    @details change light state to turn on
*/
void turn_on_light();

/*
    @details change light state to blanking
*/
void turn_blanking_light();

/*
    @details change pwm light output in blanking mode
*/
void turn_blanking_light();

/*
    @details update light control state
*/
void update_light_control_state(bool light_is_on);

void dynamic_light_blanking();

void initLightControl(void) {
    // init gpio for light control
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    // init static variables
    second_count_to_change_light_state = ProjectConst::kLightControlSecondsBetweenChangeLightMode;
    count_of_second_in_night = 0;
    light_state = LightState::Off;
}

void periodicUpdateLightControl(bool turn_on_light) {
    // decrement tick count to change light state
    second_count_to_change_light_state -= 1;
    // when tick count to change state is equal to 0, update light state
    if (second_count_to_change_light_state == 0) {
        // reset tick count to change light state
        second_count_to_change_light_state = ProjectConst::kLightControlSecondsBetweenChangeLightMode;
        // function to update light state
        update_light_control_state(turn_on_light);
    }
    // update counter of seconds in night
    if(turn_on_light == true) {
        count_of_second_in_night += 1;
        // if counter of seconds in night is higher than max value, reset board
        if (count_of_second_in_night > ProjectConst::kLightControlNightMaximumSecondDuration) {
            // TODO: reset board
        }
    }
    // when light state is blanking, update light output
    if (light_state == LightState::Blanking) {
        dynamic_light_blanking();
    }
}

LightState getLightState(void) {
    return light_state;
}

void turn_off_light() {
    light_state = LightState::Off;
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    digitalWrite(LIGHT_OUT_PIN, LOW);
    digitalWrite(ON_BOARD_LED_PIN, HIGH);
}

void turn_on_light() {
    light_state = LightState::On;
    pinMode(LIGHT_OUT_PIN, OUTPUT);
    digitalWrite(LIGHT_OUT_PIN, HIGH);
    digitalWrite(ON_BOARD_LED_PIN, LOW);
}

void turn_blanking_light() {
    pinMode(LIGHT_OUT_PIN, PWM);
    light_state = LightState::Blanking;
    dynamic_light_blanking();
}

void dynamic_light_blanking() {
    uint16_t max_duty_cycle_value = numeric_limits<uint16_t>::max();
    uint16_t diff = (uint16_t)max_duty_cycle_value *float(1 - ProjectConst::kLightControlMinimumDutyCycle);
    float fill_factor = 1 - float(count_of_second_in_night - ProjectConst::kLightControlSecondsToTurnOffLights)
                        /ProjectConst::kLightControlSecondsToBlankingLight;
    uint16_t duty_cycle  = max_duty_cycle_value - fill_factor * diff;
    pwmWrite(LIGHT_OUT_PIN, duty_cycle);
}

void update_light_control_state(bool light_is_on) {
    // when flag to turn on light is false, turn off light and reset counter of seconds in night
    if (light_is_on == false) {
        count_of_second_in_night = 0;
        if (light_state != LightState::Off) {
            turn_off_light();
        }
        return;
    }
    // when flag to turn on light is true, select light state
    if (light_state == LightState::Off && count_of_second_in_night < ProjectConst::kLightControlSecondsToTurnOffLights) {
        turn_on_light();
        return;
    }
    if (light_state == LightState::On && count_of_second_in_night < ProjectConst::kLightControlSecondsToTurnOffLights + ProjectConst::kLightControlSecondsToBlankingLight) {
        turn_blanking_light();
        return;
    }
    if (count_of_second_in_night > ProjectConst::kLightControlSecondsToTurnOffLights + ProjectConst::kLightControlSecondsToBlankingLight) {
        turn_off_light();
        return;
    }
}
