/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "DocumentUri.h"
#include "Range.h"

namespace lscpp {
namespace protocol {

struct Location {
  DocumentUri uri;
  Range range;
};

} // namespace protocol
} // namespace lscpp
