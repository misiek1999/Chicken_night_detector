#pragma once

#include <embedded_cli.h>
#include <etl/vector.h>
#include <cli_interface.h>

namespace CLI
{

extern etl::vector<CliCommandBinding, CLI::kMaxBindingCount> cli_callbacks;

} // namespace CLI
