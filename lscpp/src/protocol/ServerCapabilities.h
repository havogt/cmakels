#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace lscpp {
namespace protocol {

enum class TextDocumentSyncKind {
  /**
   * Documents should not be synced at all.
   */
  None = 0,

  /**
   * Documents are synced by always sending the full content
   * of the document.
   */
  Full = 1,

  /**
   * Documents are synced by sending the full content on open.
   * After that only incremental updates to the document are
   * send.
   */
  Incremental = 2
};

struct TextDocumentSyncOptions {
  /**
   * Open and close notifications are sent to the server. If omitted open close
   * notification should not be sent.
   */
  bool openClose;
  /**
   * Change notifications are sent to the server. See TextDocumentSyncKind.None,
   * TextDocumentSyncKind.Full and TextDocumentSyncKind.Incremental. If omitted
   * it defaults to TextDocumentSyncKind.None.
   */
  TextDocumentSyncKind change = TextDocumentSyncKind::None;

  /**
   * If present will save notifications are sent to the server. If omitted the
   * notification should not be sent.
   */
  bool willSave = false;
  /**
   * If present will save wait until requests are sent to the server. If omitted
   * the request should not be sent.
   */
  bool willSaveWaitUntil = false;
  /**
   * If present save notifications are sent to the server. If omitted the
   * notification should not be sent.
   */
  // save ?: SaveOptions; TODO
};

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
   * defining each notification [or for backwards compatibility the
   * TextDocumentSyncKind number (not implemented)]. If omitted it defaults to
   * `TextDocumentSyncKind.None`.
   */
  std::optional<TextDocumentSyncOptions> textDocumentSync;

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
