#pragma once

#include "../../external/json.hpp"
#include <loguru.hpp>

#include "../protocol/CompletionItem.h"
#include "../protocol/DidChangeTextDocumentParams.h"
#include "../protocol/DidCloseTextDocumentParams.h"
#include "../protocol/DidOpenTextDocumentParams.h"
#include "../protocol/DidSaveTextDocumentParams.h"
#include "../protocol/Hover.h"
#include "../protocol/InitializeParams.h"
#include "../protocol/InitializeResult.h"
#include "../protocol/Range.h"
#include "../protocol/TextDocumentIdentifier.h"
#include "../protocol/TextDocumentItem.h"

// serialization for std::variant
namespace nlohmann {
template <typename... Args> struct adl_serializer<std::variant<Args...>> {
  static void to_json(json &j, std::variant<Args...> const &v) {
    std::visit([&](auto &&value) { j = std::forward<decltype(value)>(value); },
               v);
  }
};
} // namespace nlohmann

namespace lscpp {
namespace protocol {
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
  // j.at("textDocument").get_to(p.textDocument);
  // j.at("position").get_to(p.position);
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

void to_json(nlohmann::json &j, const ServerCapabilities &m) {
  j = nlohmann::json{};
  j.emplace("hoverProvider", m.hoverProvider);
  if (m.completionProvider) {
    j.emplace("completionProvider", m.completionProvider.value());
  }
  if (m.textDocumentSync) {
    j.emplace("textDocumentSync", m.textDocumentSync.value());
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

// void to_json(nlohmann::json &j, const std::vector<CompletionItem> &m) {
//   LOG_F(INFO, "Serializing a vector");
//   j = m;
// }

void to_json(nlohmann::json &j, const InitializeResult &m) {
  j = nlohmann::json{};
  j.emplace("capabilities", m.capabilities);
}

void to_json(nlohmann::json &j, const Hover &m) {
  j = nlohmann::json{};
  j.emplace("contents", m.contents);
  if (m.range) {
    // j.emplace("range", m.range.value()); // TODO
  }
}

} // namespace protocol
} // namespace lscpp
