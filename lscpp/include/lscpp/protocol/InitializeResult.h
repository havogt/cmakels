/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "ServerCapabilities.h"

namespace lscpp {
namespace protocol {

struct InitializeResult {
  ServerCapabilities capabilities;
};

} // namespace protocol
} // namespace lscpp
