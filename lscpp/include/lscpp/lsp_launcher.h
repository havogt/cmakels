#pragma once

#include "lsp_server.h"
#include "stdio_transporter.h"
#include "transporter.h"

namespace lscpp {

struct launch_config {
  int startup_delay = 0; // in seconds
};

void launch(lsp_server &&server,       //
            launch_config config = {}, //
            transporter &&transporter_ = stdio_transporter{});

} // namespace lscpp
