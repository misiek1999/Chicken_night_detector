#include "system_functions.h"
#include <Arduino.h>

void System::resetSystem(void)
{

}

void System::delayMs(const unsigned int &delay_time_ms)
{
    delayMicroseconds(delay_time_ms);
}

unsigned int System::readCurrentSystemTimeMs(void)
{
    return millis();
}
