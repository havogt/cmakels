#pragma once

#include "Position.h"
#include "TextDocumentIdentifier.h"

namespace lscpp {
namespace protocol {

struct TextDocumentPositionParams {
  /**
   * The text document.
   */
  TextDocumentIdentifier textDocument;

  /**
   * The position inside the text document.
   */
  Position position;
};

} // namespace protocol
} // namespace lscpp
