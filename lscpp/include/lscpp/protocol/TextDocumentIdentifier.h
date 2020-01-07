#pragma once

#include "DocumentUri.h"

namespace lscpp {
namespace protocol {

struct TextDocumentIdentifier {
  /**
   * The text document's URI.
   */
  DocumentUri uri;
};

} // namespace protocol
} // namespace lscpp
