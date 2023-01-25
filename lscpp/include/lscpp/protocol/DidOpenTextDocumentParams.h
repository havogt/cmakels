/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
