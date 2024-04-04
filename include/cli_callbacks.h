#pragma once

#include <embedded_cli.h>
#include <etl/array.h>
#include <cli_interface.h>

namespace CLI
{
using CliCommandContainer = etl::array<CliCommandBinding, CLI::kMaxBindingCount>;
extern CliCommandContainer cli_callbacks;

}  // namespace CLI
