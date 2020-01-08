/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "TextDocumentPositionParams.h"

namespace lscpp {
namespace protocol {

struct CompletionParams : public TextDocumentPositionParams {
  /**
   * The completion context. This is only available if the client specifies
   * to send this using
   * `ClientCapabilities.textDocument.completion.contextSupport === true`
   */
  // context?: CompletionContext;
};

} // namespace protocol
} // namespace lscpp
