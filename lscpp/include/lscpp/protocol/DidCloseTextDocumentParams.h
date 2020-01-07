#pragma once

#include "TextDocumentIdentifier.h"

namespace lscpp {
namespace protocol {

struct DidCloseTextDocumentParams {
  /**
   * The document that was opened.
   */
  TextDocumentIdentifier textDocument;
};

} // namespace protocol
} // namespace lscpp
