#pragma once

#include "TextDocumentItem.h"

namespace lscpp {
namespace protocol {

struct DidOpenTextDocumentParams {
  /**
   * The document that was opened.
   */
  TextDocumentItem textDocument;
};

} // namespace protocol
} // namespace lscpp
