#pragma once

#include "TextDocumentIdentifier.h"
#include <optional>

namespace lscpp {
namespace protocol {

struct DidSaveTextDocumentParams {
  /**
   * The document that was saved.
   */
  TextDocumentIdentifier textDocument;

  /**
   * Optional the content when saved. Depends on the includeText value
   * when the save notification was requested.
   */
  std::optional<std::string> text;
};

} // namespace protocol
} // namespace lscpp
