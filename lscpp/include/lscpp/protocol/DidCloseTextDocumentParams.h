/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "TextDocumentIdentifier.h"

namespace lscpp {
namespace protocol {

struct DidCloseTextDocumentParams {
  /**
   * The document that was opened.
   */
  TextDocumentIdentifier textDocument;
};

} // namespace protocol
} // namespace lscpp
