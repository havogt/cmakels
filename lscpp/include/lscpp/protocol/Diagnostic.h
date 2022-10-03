/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "lscpp/protocol/Range.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace lscpp {
namespace protocol {

enum class DiagnosticSeverity {
  /**
   * Reports an error.
   */
  Error = 1,
  /**
   * Reports a warning.
   */
  Warning = 2,
  /**
   * Reports an information.
   */
  Information = 3,
  /**
   * Reports a hint.
   */
  Hint = 4
};

/**
 * The diagnostic tags.
 *
 * @since 3.15.0
 */
enum class DiagnosticTag {
  /**
   * Unused or unnecessary code.
   *
   * Clients are allowed to render diagnostics with this tag faded out
   * instead of having an error squiggle.
   */
  Unnecessary = 1,

  /**
   * Deprecated or obsolete code.
   *
   * Clients are allowed to rendered diagnostics with this tag strike through.
   */
  Deprecated = 2
};

struct Diagnostic {
  /**
   * The range at which the message applies.
   */
  Range range;

  /**
   * The diagnostic's severity. Can be omitted. If omitted it is up to the
   * client to interpret diagnostics as error, warning, info or hint.
   */
  std::optional<DiagnosticSeverity> severity;

  /**
   * The diagnostic's code, which might appear in the user interface.
   */
  std::optional<std::variant<int, std::string>> code;

  /**
   * An optional property to describe the error code.
   *
   * @since 3.16.0
   */
  //   std::optional<CodeDescription> codeDescription;

  /**
   * A human-readable string describing the source of this
   * diagnostic, e.g. 'typescript' or 'super lint'.
   */
  std::optional<std::string> source;

  /**
   * The diagnostic's message.
   */
  std::string message;

  /**
   * Additional metadata about the diagnostic.
   *
   * @since 3.15.0
   */
  std::optional<std::vector<DiagnosticTag>> tags;

  /**
   * An array of related diagnostic information, e.g. when symbol-names within
   * a scope collide all definitions can be marked via this property.
   */
  //   std::optional<std::vector<DiagnosticRelatedInformation>>
  //   relatedInformation;

  /**
   * A data entry field that is preserved between a
   * `textDocument/publishDiagnostics` notification and
   * `textDocument/codeAction` request.
   *
   * @since 3.16.0
   */
  //   data ?: unknown;
};

} // namespace protocol
} // namespace lscpp
