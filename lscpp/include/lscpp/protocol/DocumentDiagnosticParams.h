/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "PartialResultParams.h"
#include "TextDocumentIdentifier.h"
#include "WorkDoneProgressParams.h"
#include <optional>

namespace lscpp {
namespace protocol {

/**
 * Parameters of the document diagnostic request.
 *
 * @since 3.17.0
 */
struct DocumentDiagnosticParams : WorkDoneProgressParams, PartialResultParams {
  /**
   * The text document.
   */
  TextDocumentIdentifier textDocument;

  /**
   * The additional identifier provided during registration.
   */
  std::optional<std::string> identifier;

  /**
   * The result id of a previous response if provided.
   */
  std::optional<std::string> previousResultId;
};

} // namespace protocol
} // namespace lscpp
