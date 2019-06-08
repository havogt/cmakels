#pragma once

#include "../../external/json.hpp"
#include <loguru.hpp>

#include "../protocol/Hover.h"
#include "../protocol/InitializeParams.h"
#include "../protocol/InitializeResult.h"
#include "../protocol/Range.h"

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
  }
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

void to_json(nlohmann::json &j, const ServerCapabilities &m) {
  j = nlohmann::json{};
  j.emplace("hoverProvider", m.hoverProvider);
  // j.emplace("definitionProvider", m.getDefinitionProvider());
}

void to_json(nlohmann::json &j, const InitializeResult &m) {
  j = nlohmann::json{};
  j.emplace("capabilities", m.capabilities);
  //   j.emplace("hoverProvider", m.getHoverProvider());
  // nlohmann::json j{"capabilities", capabilities_};
}

std::string contents; // TODO
//   contents : MarkedString | MarkedString[] | MarkupContent;

/**
 * An optional range is a range inside a text document
 * that is used to visualize a hover, e.g. by changing the background color.
 */
std::optional<Range> range;

void to_json(nlohmann::json &j, const Hover &m) {
  j = nlohmann::json{};
  j.emplace("contents", m.contents);
  if (m.range) {
    // j.emplace("range", m.range.value()); // TODO
  }
}

} // namespace protocol
} // namespace lscpp
