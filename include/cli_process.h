#pragma once

#include <Arduino.h>
#include <cli_interface.h>

namespace CLI
{
    
class CLIProcess
{
public:
    CLIProcess();

    // Update CLI interfaces periodically
    bool periodicCProcessCLI();

private:
    CLIInterface cli_interface_;
};

} // namespace CLI
