#include "cli_process.h"
#include <Arduino.h>
#include "cli_callbacks.h"

int serial_available_wrapper() {
    return Serial.available();
}

void serial_write_wrapper(EmbeddedCli * cli, char c) {
    (void) cli;
    Serial.write(c);
}

char serial_read_wrapper() {
    return static_cast<char>(Serial.read());
}

CLI::CLIProcess::CLIProcess():
    cli_interface_(CLI::cli_callbacks, serial_read_wrapper,
                   serial_write_wrapper, serial_available_wrapper) {
}

bool CLI::CLIProcess::periodicCProcessCLI() {
    return cli_interface_.periodicCProcessCLI();
}
