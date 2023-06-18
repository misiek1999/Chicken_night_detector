#ifndef CLI_INTERFACE_H
#define CLI_INTERFACE_H

#include <Arduino.h>
#include <embedded_cli.h>
#include <rtc_controler.h>
#include <light_control.h>

namespace CLI {
// const values used in cli interface
constexpr uint16_t kBufferSize = 512;
constexpr uint16_t kMaxBindingCount = 8;
constexpr uint16_t kHistorySize = 32;
constexpr uint16_t kCmdBuffersize = 64;
constexpr uint16_t kRxBufferSize = 32;

/*
    @details function to initialize CLI
*/
void init_CLI(void);

/*
    @details function to process CLI in main loop
*/
void periodicCProcessCLI(void);

} // namespace CLI

#endif // CLI_INTERFACE_H