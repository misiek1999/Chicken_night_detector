/*
    This file include function to control light
*/
#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#include <Arduino.h>
#include "project_pin_definition.h"
#include "project_const.h"

// Enum class for light mode machine state
enum class LightState
{
    Off,        // Default state
    Blanking,   // Light is slowly blanking
    On          // Light is on
};

/*
    @details function to init light controller
*/
void initLightControl(void);

/*
    @details function to update state of light controller
    call this function in constant period of time (1s)
    @param turn_on_light true if light should be turn on
*/
void perdiocUpdateLightControl(bool turn_on_light);

/*
    @details function to get current state of light controller
*/
LightState getLightState(void);

#endif // LIGHT_CONTROL_H