/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
