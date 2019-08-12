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
