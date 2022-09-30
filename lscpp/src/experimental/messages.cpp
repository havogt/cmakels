/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "lscpp/experimental/messages.h"

#include <cstddef>
#include <sstream>

#include "../external/json.hpp"
#include "../protocol_serializer/serializer.h"

namespace lscpp::experimental {
namespace {
const std::map<std::string, method_kind> method_str_to_kind{
    {"initialize", method_kind::INITIALIZE},
    {"initialized", method_kind::INITIALIZED},
    {"exit", method_kind::EXIT},
    {"shutdown", method_kind::SHUTDOWN},
    {"textDocument/didOpen", method_kind::TEXT_DOCUMENT_DID_OPEN},
    {"textDocument/didChange", method_kind::TEXT_DOCUMENT_DID_CHANGE},
    {"textDocument/didClose", method_kind::TEXT_DOCUMENT_DID_CLOSE},
    {"textDocument/didSave", method_kind::TEXT_DOCUMENT_DID_SAVE},
    {"textDocument/hover", method_kind::TEXT_DOCUMENT_HOVER},
    {"textDocument/definition", method_kind::TEXT_DOCUMENT_DEFINITION},
    {"textDocument/completion", method_kind::TEXT_DOCUMENT_COMPLETION},
};
}

void write_lsp_message(transporter &t, std::string const &content) {
  std::stringstream content_length;
  content_length << "Content-Length: ";
  content_length << content.size();
  t.write_message(content_length.str(), true);
  t.write_message("", true);

  t.write_message(content);
}

std::string make_notification_message(std::string const &msg) {
  nlohmann::json j{{"jsonrpc", "2.0"}, {"method", "window/showMessage"}};
  nlohmann::json params{};
  params.emplace("type", 3);
  params.emplace("message", msg);
  j.emplace("params", params);
  //  {"parameters", {"type", "3"}, {"message", msg}} };
  return j.dump();
}

template <typename Result>
std::string response_message(int id, Result const &result) {
  return nlohmann::json{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}}
      .dump();
}

std::string response_message(int id) {
  return nlohmann::json{
      {"jsonrpc", "2.0"}, {"id", id}, {"result", std::nullptr_t{}}}
      .dump();
}

template std::string response_message(int id,
                                      const protocol::InitializeResult &result);
template std::string response_message(int id, const protocol::Hover &result);
template std::string response_message(int id, const protocol::Location &result);
template std::string
response_message(int id, const std::vector<protocol::CompletionItem> &result);

namespace {
template <class Params> auto as_request_message(nlohmann::json const &j) {
  return message{method_str_to_kind.at(j["method"]), j["id"],
                 j["params"].get<Params>()};
}
auto as_request_message(nlohmann::json const &j) {
  return message{method_str_to_kind.at(j["method"]), j["id"]};
}
template <class Params> auto as_notification_message(nlohmann::json const &j) {
  return message{method_str_to_kind.at(j["method"]), -1,
                 j["params"].get<Params>()};
}
auto as_notification_message(nlohmann::json const &j) {
  return message{method_str_to_kind.at(j["method"]), -1};
}

struct no_params {};

template <method_kind Kind> struct to_param;
template <> struct to_param<method_kind::INITIALIZE> {
  using type = protocol::InitializeParams;
};
template <> struct to_param<method_kind::INITIALIZED> { using type = void; };
template <> struct to_param<method_kind::EXIT> { using type = void; };
template <> struct to_param<method_kind::SHUTDOWN> { using type = void; };
template <> struct to_param<method_kind::TEXT_DOCUMENT_HOVER> {
  using type = protocol::TextDocumentPositionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DEFINITION> {
  using type = protocol::TextDocumentPositionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_COMPLETION> {
  using type = protocol::CompletionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_OPEN> {
  using type = protocol::DidOpenTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_CHANGE> {
  using type = protocol::DidChangeTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_CLOSE> {
  using type = protocol::DidCloseTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_SAVE> {
  using type = protocol::DidSaveTextDocumentParams;
};

template <method_kind KIND> using to_params_t = typename to_param<KIND>::type;

template <class Params> auto params_impl(nlohmann::json const &j) {
  return j["params"].get<Params>();
}

template <> auto params_impl<void>(nlohmann::json const &j) {
  return std::any{};
}

template <method_kind Kind> auto as_message(nlohmann::json const &j) {
  return message{Kind, j.contains("id") ? static_cast<int>(j["id"]) : -1,
                 params_impl<to_params_t<Kind>>(j)};
}

} // namespace

message parse_request(std::string request) {
  nlohmann::json j = nlohmann::json::parse(request);
  const auto method = method_str_to_kind.at(j["method"]);

  // TODO fix this pattern
  if (method == method_kind::INITIALIZE) {
    return as_message<method_kind::INITIALIZE>(j);
  } else if (method == method_kind::INITIALIZED) {
    return as_message<method_kind::INITIALIZED>(j);
  } else if (method == method_kind::EXIT) {
    return as_message<method_kind::EXIT>(j);
  } else if (method == method_kind::SHUTDOWN) {
    return as_message<method_kind::SHUTDOWN>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_HOVER) {
    return as_message<method_kind::TEXT_DOCUMENT_HOVER>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_DEFINITION) {
    return as_message<method_kind::TEXT_DOCUMENT_DEFINITION>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_COMPLETION) {
    return as_message<method_kind::TEXT_DOCUMENT_COMPLETION>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_DID_OPEN) {
    return as_message<method_kind::TEXT_DOCUMENT_DID_OPEN>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_DID_CHANGE) {
    return as_message<method_kind::TEXT_DOCUMENT_DID_CHANGE>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_DID_CLOSE) {
    return as_message<method_kind::TEXT_DOCUMENT_DID_CLOSE>(j);
  } else if (method == method_kind::TEXT_DOCUMENT_DID_SAVE) {
    return as_message<method_kind::TEXT_DOCUMENT_DID_SAVE>(j);
    // } else if (method == "textDocument/willSave") {
    //   TODO
  }
  assert(false); // unreachable
}

} // namespace lscpp::experimental
