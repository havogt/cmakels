/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "DocumentUri.h"

namespace lscpp {
namespace protocol {

struct TextDocumentIdentifier {
  /**
   * The text document's URI.
   */
  DocumentUri uri;
};

} // namespace protocol
} // namespace lscpp
