#include "cli_interface.h"

CLI::CLIInterface::CLIInterface(etl::vector<CliCommandBinding, kMaxBindingCount> &cli_callbacks,
                                serial_read_char_fun_t getCharFromSerial,
                                serial_write_char_fun_t sendCharOverSerial,
                                serial_rest_bytes_in_rx_buffer_fun_t restBytesInRxBuffer):
    sendCharOverSerial_(sendCharOverSerial),
    getCharFromSerial_(getCharFromSerial),
    restBytesInRxBuffer_(restBytesInRxBuffer)
{
    // create new instance of CLI config
    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->cliBuffer = cliBuffer;
    config->maxBindingCount = CLI::kMaxBindingCount;
    config->historyBufferSize = CLI::kHistorySize;
    config->cmdBufferSize = CLI::kCmdBuffersize;
    config->cliBufferSize = CLI::kBufferSize;
    config->rxBufferSize = CLI::kRxBufferSize;
    // create new instance of CLI
    cli_ = embeddedCliNew(config);
    // check cli creation status
    if (cli_ == NULL) {
        Serial.println(F("Cli was not created. Check sizes!"));
        uint16_t size = embeddedCliRequiredSize(config);
        Serial.print(F("Required size: "));
        Serial.println(size);
    }
    else{
        // add cli function to send char over serial
        cli_->writeChar = sendCharOverSerial_;
        // add cli function callbacks
        for (auto &callback : cli_callbacks) {
            embeddedCliAddBinding(cli_, callback);
        }
    }
}

bool CLI::CLIInterface::periodicCProcessCLI()
{
    if (cli_ == NULL) {
        return false;
    }
    // read received bytes from buffer
    while (restBytesInRxBuffer_() > 0) {
        embeddedCliReceiveChar(cli_, getCharFromSerial_());
    }
    // process received data from serial
    embeddedCliProcess(cli_);
    return true;
}
