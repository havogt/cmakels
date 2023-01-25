/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "Position.h"

namespace lscpp {
namespace protocol {

struct Range {
  /**
   * The range's start position.
   */
  Position start;

  /**
   * The range's end position.
   */
  Position end;
};

} // namespace protocol
} // namespace lscpp
