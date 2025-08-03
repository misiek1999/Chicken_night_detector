#pragma once

#include <embedded_cli.h>
#include <etl/vector.h>
#include <functional>

// enable embedded CLI
#define EMBEDDED_CLI_IMPL

namespace CLI
{

// const values used in cli interface
constexpr uint16_t kBufferSize = 640;
constexpr uint16_t kMaxBindingCount = 16;
constexpr uint16_t kHistorySize = 64;
constexpr uint16_t kCmdBuffersize = 64;
constexpr uint16_t kRxBufferSize = 32;

typedef char (*serial_read_char_fun_t)(void);
typedef void (*serial_write_char_fun_t)(EmbeddedCli *, char);
typedef int (*serial_rest_bytes_in_rx_buffer_fun_t)(void);

class CLIInterface
{
public:
    CLIInterface(etl::array<CliCommandBinding, kMaxBindingCount> &cli_callbacks,
                serial_read_char_fun_t getCharFromSerial,
                serial_write_char_fun_t sendCharOverSerial,
                serial_rest_bytes_in_rx_buffer_fun_t restBytesInRxBuffer);

    // Update CLI interfaces periodically
    bool periodicCProcessCLI();

private:
    // buffer for cli
    CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI::kBufferSize)];

    // cli instance
    EmbeddedCli *cli_;

    // callback functions to send char over serial
    serial_write_char_fun_t sendCharOverSerial_;
    // callback functions to get char from serial
    serial_read_char_fun_t getCharFromSerial_;
    // callback functions to get rest bytes is in rx buffer
    serial_rest_bytes_in_rx_buffer_fun_t restBytesInRxBuffer_;
};

}   // namespace CLI
