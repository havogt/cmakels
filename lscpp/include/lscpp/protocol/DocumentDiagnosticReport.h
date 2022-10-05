/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "Diagnostic.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace lscpp {
namespace protocol {

/**
 * The document diagnostic report kinds.
 *
 * @since 3.17.0
 */
enum class DocumentDiagnosticReportKind {
  /**
   * A diagnostic report with a full
   * set of problems.
   */
  Full,

  /**
   * A report indicating that the last
   * returned report is still accurate.
   */
  Unchanged
};

inline constexpr const char *to_string(DocumentDiagnosticReportKind kind) {
  switch (kind) {
  case DocumentDiagnosticReportKind::Full:
    return "full";
  case DocumentDiagnosticReportKind::Unchanged:
    return "unchanged";
  }
}

/**
 * A diagnostic report with a full set of problems.
 *
 * @since 3.17.0
 */
struct FullDocumentDiagnosticReport {
  /**
   * A full document diagnostic report.
   */
  static constexpr DocumentDiagnosticReportKind kind =
      DocumentDiagnosticReportKind::Full;

  /**
   * An optional result id. If provided it will
   * be sent on the next diagnostic request for the
   * same document.
   */
  std::optional<std::string> resultId;

  /**
   * The actual items.
   */
  std::vector<Diagnostic> items;
};

/**
 * A diagnostic report indicating that the last returned
 * report is still accurate.
 *
 * @since 3.17.0
 */
struct UnchangedDocumentDiagnosticReport {
  /**
   * A document diagnostic report indicating
   * no changes to the last result. A server can
   * only return `unchanged` if result ids are
   * provided.
   */
  static constexpr DocumentDiagnosticReportKind kind =
      DocumentDiagnosticReportKind::Unchanged;

  /**
   * A result id which will be sent on the next
   * diagnostic request for the same document.
   */
  std::string resultId;
};

/**
 * A full diagnostic report with a set of related documents.
 *
 * @since 3.17.0
 */
struct RelatedFullDocumentDiagnosticReport : FullDocumentDiagnosticReport {
  /**
   * Diagnostics of related documents. This information is useful
   * in programming languages where code in a file A can generate
   * diagnostics in a file B which A depends on. An example of
   * such a language is C/C++ where marco definitions in a file
   * a.cpp and result in errors in a header file b.hpp.
   *
   * @since 3.17.0
   */
  //   std::optional<std::unordered_map<std::string /* DocumentUri */,
  //                      std::variant<FullDocumentDiagnosticReport,
  //                                   UnchangedDocumentDiagnosticReport>>>
  //       relatedDocuments;
};

/**
 * An unchanged diagnostic report with a set of related documents.
 *
 * @since 3.17.0
 */
struct RelatedUnchangedDocumentDiagnosticReport
    : UnchangedDocumentDiagnosticReport {
  /**
   * Diagnostics of related documents. This information is useful
   * in programming languages where code in a file A can generate
   * diagnostics in a file B which A depends on. An example of
   * such a language is C/C++ where marco definitions in a file
   * a.cpp and result in errors in a header file b.hpp.
   *
   * @since 3.17.0
   */
  std::optional<
      std::unordered_map<std::string /* DocumentUri */,
                         std::variant<FullDocumentDiagnosticReport,
                                      UnchangedDocumentDiagnosticReport>>>
      relatedDocuments;
};

/**
 * The result of a document diagnostic pull request. A report can
 * either be a full report containing all diagnostics for the
 * requested document or a unchanged report indicating that nothing
 * has changed in terms of diagnostics in comparison to the last
 * pull request.
 *
 * @since 3.17.0
 */
using DocumentDiagnosticReport =
    std::variant<RelatedFullDocumentDiagnosticReport,
                 RelatedUnchangedDocumentDiagnosticReport>;

} // namespace protocol
} // namespace lscpp
