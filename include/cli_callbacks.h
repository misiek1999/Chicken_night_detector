#pragma once

#include <embedded_cli.h>
#include <etl/array.h>
#include <cli_interface.h>

namespace CLI
{

extern etl::array<CliCommandBinding, CLI::kMaxBindingCount> cli_callbacks;

} // namespace CLI
