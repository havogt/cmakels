/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

/**
 * Save options.
 */
struct SaveOptions {
  /**
   * The client is supposed to include the content on save.
   */
  bool includeText = false;
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
  std::optional<SaveOptions> save;
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

struct WorkDoneProgressOptions {
  bool workDoneProgress = false;
};

struct DiagnosticOptions : WorkDoneProgressOptions {
  /**
   * An optional identifier under which the diagnostics are
   * managed by the client.
   */
  std::optional<std::string> identifier;

  /**
   * Whether the language has inter file dependencies meaning that
   * editing code in one file can result in a different diagnostic
   * set in another file. Inter file dependencies are common for
   * most programming languages and typically uncommon for linters.
   */
  bool interFileDependencies = false;

  /**
   * The server provides support for workspace diagnostics as well.
   */
  bool workspaceDiagnostics = false;
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
  bool hoverProvider = false;

  /**
   * The server provides goto definition support.
   */
  bool definitionProvider = false;

  /**
   * The server provides completion support.
   */
  std::optional<CompletionOptions> completionProvider;

  /**
   * The server has support for pull model diagnostics.
   *
   * @since 3.17.0
   */
  std::optional<DiagnosticOptions>
      diagnosticProvider; // TODO or DiagnosticRegistrationOptions;
};

} // namespace protocol
} // namespace lscpp
