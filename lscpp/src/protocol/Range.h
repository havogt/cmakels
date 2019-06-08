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
