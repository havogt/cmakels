/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "Range.h"
#include "TextDocumentIdentifier.h"
#include <optional>
#include <vector>

namespace lscpp {
namespace protocol {

/**
 * An event describing a change to a text document. If range and rangeLength are
 * omitted the new text is considered to be the full content of the document.
 */
struct TextDocumentContentChangeEvent {
  /**
   * The range of the document that changed.
   */
  std::optional<Range> range;

  /**
   * The length of the range that got replaced.
   */
  std::optional<int> rangeLength;

  /**
   * The new text of the range/document.
   */
  std::string text;
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
  /**
   * The version number of this document. If a versioned text document
   * identifier is sent from the server to the client and the file is not open
   * in the editor (the server has not received an open notification before) the
   * server can send `null` to indicate that the version is known and the
   * content on disk is the truth (as speced with document content ownership).
   *
   * The version number of a document will increase after each change, including
   * undo/redo. The number doesn't need to be consecutive.
   */
  int version; // TODO null;
};

struct DidChangeTextDocumentParams {
  /**
   * The document that did change. The version number points
   * to the version after all provided content changes have
   * been applied.
   */
  VersionedTextDocumentIdentifier textDocument;

  /**
   * The actual content changes. The content changes describe single state
   * changes to the document. So if there are two content changes c1 and c2 for
   * a document in state S then c1 move the document to S' and c2 to S''.
   */
  std::vector<TextDocumentContentChangeEvent> contentChanges;
};

} // namespace protocol
} // namespace lscpp
