/*
    Set of system function
*/
#pragma once

namespace System
{
    /*
        @details function to reset mcu
    */
    void resetSystem(void);

    /*
        @details delay function in ms
    */
    void delayMs(const unsigned int &delay_time_ms);

    /*
        @details function to read current system time in ms
    */
    unsigned int readCurrentSystemTimeMs(void);

} // namespace System
