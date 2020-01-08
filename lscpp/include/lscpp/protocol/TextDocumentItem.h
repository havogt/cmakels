/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "DocumentUri.h"
#include <string>

namespace lscpp {
namespace protocol {

struct TextDocumentItem {
  /**
   * The text document's URI.
   */
  DocumentUri uri;

  /**
   * The text document's language identifier.
   */
  std::string languageId;

  /**
   * The version number of this document (it will increase after each
   * change, including undo/redo).
   */
  int version;

  /**
   * The content of the opened text document.
   */
  std::string text;
};

} // namespace protocol
} // namespace lscpp
