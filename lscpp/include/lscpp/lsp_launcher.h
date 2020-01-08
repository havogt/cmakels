/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "lsp_server.h"
#include "stdio_transporter.h"
#include "transporter.h"

namespace lscpp {

// duplicated from loguru to avoid exposing loguru in the public interface
using Verbosity = int;
enum NamedVerbosity : Verbosity {
  Verbosity_INVALID = -10,
  Verbosity_OFF = -9,
  Verbosity_FATAL = -3,
  Verbosity_ERROR = -2,
  Verbosity_WARNING = -1,
  Verbosity_INFO = 0,
  Verbosity_0 = 0,
  Verbosity_1 = +1,
  Verbosity_2 = +2,
  Verbosity_3 = +3,
  Verbosity_4 = +4,
  Verbosity_5 = +5,
  Verbosity_6 = +6,
  Verbosity_7 = +7,
  Verbosity_8 = +8,
  Verbosity_9 = +9,
  Verbosity_MAX = +9,
};

struct logger_config {
  NamedVerbosity verbosity = Verbosity_OFF;
  std::string filename;
};

struct launch_config {
  int startup_delay = 0; // in seconds
  logger_config logger;
};

void launch(lsp_server &&server,       //
            launch_config config = {}, //
            transporter &&transporter_ = stdio_transporter{});

} // namespace lscpp
