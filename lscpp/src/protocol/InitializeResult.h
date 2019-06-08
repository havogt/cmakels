#pragma once

#include "ServerCapabilities.h"

namespace lscpp {
namespace protocol {

struct InitializeResult {
  ServerCapabilities capabilities;
};

} // namespace protocol
} // namespace lscpp
