/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "../../external/json.hpp"
#include <loguru.hpp>
#include <string>

#include "lscpp/protocol/CompletionItem.h"
#include "lscpp/protocol/CompletionParams.h"
#include "lscpp/protocol/Diagnostic.h"
#include "lscpp/protocol/DidChangeTextDocumentParams.h"
#include "lscpp/protocol/DidCloseTextDocumentParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/DidSaveTextDocumentParams.h"
#include "lscpp/protocol/DocumentDiagnosticParams.h"
#include "lscpp/protocol/DocumentDiagnosticReport.h"
#include "lscpp/protocol/Hover.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/Location.h"
#include "lscpp/protocol/PartialResultParams.h"
#include "lscpp/protocol/Range.h"
#include "lscpp/protocol/ServerCapabilities.h"
#include "lscpp/protocol/TextDocumentIdentifier.h"
#include "lscpp/protocol/TextDocumentItem.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"

// serialization for std::variant
namespace nlohmann {
template <typename... Args> struct adl_serializer<std::variant<Args...>> {
  static void to_json(json &j, std::variant<Args...> const &v) {
    std::visit([&](auto &&value) { j = std::forward<decltype(value)>(value); },
               v);
  }
};

// for ProgressToken (but it's an alias for variant<int, std::string>, therefore
// it applies to all other variant<int,std::string> as well)
template <> struct adl_serializer<std::variant<int, std::string>> {
  // duplicated from the generic one
  static void to_json(json &j, std::variant<int, std::string> const &v) {
    std::visit([&](auto &&value) { j = std::forward<decltype(value)>(value); },
               v);
  }

  static void from_json(const nlohmann::json &j,
                        std::variant<int, std::string> &p) {
    if (j.is_string()) {
      j.get_to(std::get<std::string>(p));
    } else {
      assert(j.is_number());
      j.get_to(std::get<int>(p));
    }
  }
};
} // namespace nlohmann

namespace lscpp {
namespace protocol {
namespace serializer_impl_ {

template <class T>
void from_optional(const nlohmann::json &j, std::string const &key,
                   std::optional<T> &to) {
  if (j.contains(key)) {
    T v;
    j[key].get_to(v);
    to = v;
  }
}

} // namespace serializer_impl_

#define FROM_OPTIONAL(key) serializer_impl_::from_optional(j, #key, p.key)

void from_json(const nlohmann::json &j,
               protocol::WorkspaceClientCapabilities &p) {}

void from_json(const nlohmann::json &j,
               protocol::TextDocumentClientCapabilities &p) {}

void from_json(const nlohmann::json &j, protocol::ClientCapabilities &p) {
  //   from_json(j, static_cast<Message&>(m));
  if (j.find("workspace") != j.end()) {
    protocol::WorkspaceClientCapabilities workspace;
    j["workspace"].get_to(workspace);
    p.workspace = workspace;
  }
  if (j.find("textDocument") != j.end()) {
    protocol::TextDocumentClientCapabilities textDocument;
    j["textDocument"].get_to(textDocument);
    p.textDocument = textDocument;
  }
}

void from_json(const nlohmann::json &j, protocol::TextDocumentItem &p) {
  j.at("uri").get_to(p.uri);
  j.at("languageId").get_to(p.languageId);
  j.at("version").get_to(p.version);
  j.at("text").get_to(p.text);
}

void from_json(const nlohmann::json &j,
               protocol::DidOpenTextDocumentParams &p) {
  j.at("textDocument").get_to(p.textDocument);
}

void from_json(const nlohmann::json &j, protocol::TextDocumentIdentifier &p) {
  j.at("uri").get_to(p.uri);
}

void from_json(const nlohmann::json &j,
               protocol::DidSaveTextDocumentParams &p) {
  j.at("textDocument").get_to(p.textDocument);
  if (j.find("text") != j.end()) {
    std::string text;
    j["text"].get_to(text);
    p.text = text;
  }
}

void from_json(const nlohmann::json &j,
               protocol::DidCloseTextDocumentParams &p) {
  j.at("textDocument").get_to(p.textDocument);
}

void from_json(const nlohmann::json &j,
               protocol::VersionedTextDocumentIdentifier &p) {
  from_json(j, static_cast<TextDocumentIdentifier &>(p));
  j.at("version").get_to(p.version);
}

void from_json(const nlohmann::json &j, protocol::Position &p) {
  j.at("line").get_to(p.line);
  j.at("character").get_to(p.character);
}

void from_json(const nlohmann::json &j, protocol::Range &p) {
  j.at("start").get_to(p.start);
  j.at("end").get_to(p.end);
}

void from_json(const nlohmann::json &j,
               protocol::TextDocumentContentChangeEvent &p) {
  if (j.find("range") != j.end()) {
    protocol::Range range;
    j["range"].get_to(range);
    p.range = range;
  }
  if (j.find("rangeLength") != j.end()) {
    int rangeLength;
    j["rangeLength"].get_to(rangeLength);
    p.rangeLength = rangeLength;
  }
  j.at("text").get_to(p.text);
}

void from_json(const nlohmann::json &j,
               protocol::DidChangeTextDocumentParams &p) {
  j.at("textDocument").get_to(p.textDocument);
  j.at("contentChanges").get_to(p.contentChanges);
}
void from_json(const nlohmann::json &j,
               protocol::TextDocumentPositionParams &p) {
  j.at("textDocument").get_to(p.textDocument);
  j.at("position").get_to(p.position);
}

void from_json(const nlohmann::json &j, protocol::CompletionParams &p) {
  from_json(j, static_cast<TextDocumentPositionParams &>(p));
}

void from_json(const nlohmann::json &j, protocol::DocumentDiagnosticParams &p) {
  FROM_OPTIONAL(workDoneToken);
  FROM_OPTIONAL(partialResultToken);
  j.at("textDocument").get_to(p.textDocument);
  FROM_OPTIONAL(identifier);
  FROM_OPTIONAL(previousResultId);
}

void from_json(const nlohmann::json &j, protocol::InitializeParams &p) {
  //   from_json(j, static_cast<Message&>(m));
  if (j.at("processId") != "null") {
    p.processId.emplace(j["processId"]);
  }

  if (j.find("rootPath") != j.end()) {
    LOG_F(ERROR, "Ignoring rootPath!");
  }

  if (j.at("rootUri") != "null") {
    p.rootUri.emplace(protocol::DocumentUri{j["rootUri"]});
  }

  j.at("capabilities").get_to(p.capabilities);
}

void to_json(nlohmann::json &j, const CompletionOptions &m) {
  j = nlohmann::json{};
  j.emplace("resolveProvider", m.resolveProvider);
  j.emplace("triggerCharacters", m.triggerCharacters);
}

void to_json(nlohmann::json &j, const SaveOptions &m) {
  j = nlohmann::json{};
  j.emplace("includeText", m.includeText);
}

void to_json(nlohmann::json &j, const TextDocumentSyncOptions &m) {
  j = nlohmann::json{};
  j.emplace("openClose", m.openClose);
  j.emplace("change", m.change);
  j.emplace("willSave", m.willSave);
  j.emplace("willSaveWaitUntil", m.willSaveWaitUntil);
  if (m.save)
    j.emplace("save", m.save.value());
}

void to_json(nlohmann::json &j, const DiagnosticOptions &m) {
  j = nlohmann::json{};
  j.emplace("workDoneProgress", m.workDoneProgress);
  if (m.identifier)
    j.emplace("identifier", m.identifier.value());
  j.emplace("interFileDependencies", m.interFileDependencies);
  j.emplace("workspaceDiagnostics", m.workspaceDiagnostics);
}

void to_json(nlohmann::json &j, const ServerCapabilities &m) {
  j = nlohmann::json{};
  j.emplace("hoverProvider", m.hoverProvider);
  j.emplace("definitionProvider", m.definitionProvider);
  if (m.completionProvider) {
    j.emplace("completionProvider", m.completionProvider.value());
  }
  if (m.textDocumentSync) {
    j.emplace("textDocumentSync", m.textDocumentSync.value());
  }
  if (m.diagnosticProvider) {
    j.emplace("diagnosticProvider", m.diagnosticProvider.value());
  }
}

// void to_json(nlohmann::json &j,
//              const std::variant<std::vector<CompletionItem>> &m)
// {
//   LOG_F(INFO, "Converting
//   std::variant<std::vector<protocol::CompletionItem>>"); if
//   (std::holds_alternative<std::vector<CompletionItem>>(m))
//   {
//     LOG_F(INFO, "holding alternative std::vector<CompletionItem>");
//     auto const &v = std::get<std::vector<CompletionItem>>(m);
//     // j = v;
//     LOG_F(INFO, "calling to_json");
//     nlohmann::to_json(j, v);
//     // j = std::get<std::vector<CompletionItem>>(m);
//     LOG_F(INFO, "Converted std::vector<CompletionItem>");
//   }
//   else
//   {
//     LOG_F(INFO, "Don't know what I hold!");
//   }

//   // j = nlohmann::json{};
//   // j.emplace("label", m.label);
// }

void to_json(nlohmann::json &j, const CompletionItem &m) {
  j = nlohmann::json{};
  LOG_F(INFO, "Serializing CompletionItem");
  j.emplace("label", m.label);
}

void to_json(nlohmann::json &j, const InitializeResult &m) {
  j = nlohmann::json{};
  j.emplace("capabilities", m.capabilities);
}

void to_json(nlohmann::json &j, const Hover &m) {
  j = nlohmann::json{};
  j.emplace("contents", m.contents);
  if (m.range) {
    j.emplace("range", m.range.value());
  }
}

void to_json(nlohmann::json &j, const Position &m) {
  j = nlohmann::json{};
  j.emplace("line", m.line);
  j.emplace("character", m.character);
}

void to_json(nlohmann::json &j, const Range &m) {
  j = nlohmann::json{};
  j.emplace("start", m.start);
  j.emplace("end", m.end);
}

void to_json(nlohmann::json &j, const Location &m) {
  j = nlohmann::json{};
  j.emplace("uri", m.uri);
  j.emplace("range", m.range);
}

template <class T>
void emplace_optional(nlohmann::json &j, std::string const &key,
                      std::optional<T> const &value) {
  if (value)
    j.emplace(key, value.value());
}

void to_json(nlohmann::json &j, Diagnostic const &m) {
  j = nlohmann::json{};
  j.emplace("range", m.range);
  if (m.severity)
    j.emplace("severity", m.severity.value());
  if (m.code)
    j.emplace("code", m.code.value());
  if (m.source)
    j.emplace("source", m.source.value());
  j.emplace("message", m.message);
  emplace_optional(j, "tags", m.tags); // TODO propagate this pattern
}

void to_json(nlohmann::json &j, RelatedFullDocumentDiagnosticReport const &m) {
  j = nlohmann::json{};
  j.emplace("kind", to_string(m.kind));
  if (m.resultId)
    j.emplace("resultId", m.resultId.value());
  j.emplace("items", m.items);
  // TODO relatedDocuments
}

void to_json(nlohmann::json &j,
             RelatedUnchangedDocumentDiagnosticReport const &m) {
  j = nlohmann::json{};
  j.emplace("kind", to_string(m.kind));
  j.emplace("resultId", m.resultId);
  // TODO relatedDocuments
}

} // namespace protocol
} // namespace lscpp

#undef FROM_OPTIONAL
