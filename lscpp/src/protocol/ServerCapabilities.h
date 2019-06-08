#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace lscpp {
namespace protocol {

enum class TextDocumentSyncKind { None };

struct TextDocumentSyncOptions {};

/**
 * Completion options.
 */
struct CompletionOptions {
  /**
   * The server provides support to resolve additional
   * information for a completion item.
   */
  bool resolveProvider;

  /**
   * The characters that trigger completion automatically.
   */
  std::vector<std::string> triggerCharacters;
};

struct ServerCapabilities {
  /**
   * Defines how text documents are synced. Is either a detailed structure
   * defining each notification or for backwards compatibility the
   * TextDocumentSyncKind number. If omitted it defaults to
   * `TextDocumentSyncKind.None`.
   */
  std::variant<TextDocumentSyncKind, TextDocumentSyncOptions> textDocumentSync;

  /**
   * The server provides hover support.
   */
  bool hoverProvider;

  /**
   * The server provides completion support.
   */
  std::optional<CompletionOptions> completionProvider;
};

} // namespace protocol
} // namespace lscpp
