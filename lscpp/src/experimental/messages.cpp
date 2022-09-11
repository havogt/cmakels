#include "../external/json.hpp"
#include "../protocol_serializer/serializer.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include <iostream> // TODO remove
#include <lscpp/experimental/messages.h>

namespace lscpp {

namespace {
template <typename Result>
nlohmann::json make_response_message(int id, Result const &result) {
  nlohmann::json j{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
  return j.dump();
}
} // namespace

std::string make_notification_message(std::string const &msg) {
  nlohmann::json j{{"jsonrpc", "2.0"}, {"method", "window/showMessage"}};
  nlohmann::json params{};
  params.emplace("type", 3);
  params.emplace("message", msg);
  j.emplace("params", params);
  //  {"parameters", {"type", "3"}, {"message", msg}} };
  return j.dump();
}

std::string initialize_response(int id,
                                protocol::InitializeResult const &result) {
  return make_response_message(id, result);
}
std::string hover_response(int id, protocol::Hover const &result) {
  return make_response_message(id, result);
}
std::string shutdown_response(int id) {
  nlohmann::json j_null;
  return make_response_message(id, j_null);
}

namespace {
template <class Params> auto as_request_message(nlohmann::json const &j) {
  return message{j["method"],
                 request_message{j["id"], j["params"].get<Params>()}};
}
auto as_request_message(nlohmann::json const &j) {
  return message{j["method"], request_message{j["id"]}};
}
template <class Params> auto as_notification_message(nlohmann::json const &j) {
  return message{j["method"], notification_message{j["params"].get<Params>()}};
}
auto as_notification_message(nlohmann::json const &j) {
  return message{j["method"], notification_message{}};
}
} // namespace

message parse_request(std::string request) {
  nlohmann::json j = nlohmann::json::parse(request);
  auto const &method = j["method"];

  if (method == "initialize") {
    return as_request_message<protocol::InitializeParams>(j);
  } else if (method == "initialized") {
    return as_notification_message(j);
  } else if (method == "exit") {
    return as_notification_message(j);
  } else if (method == "shutdown") {
    return as_request_message(j);
  } else if (method == "textDocument/hover") {
    return as_request_message<protocol::TextDocumentPositionParams>(j);
  } else if (method == "textDocument/didOpen") {
    return as_notification_message<protocol::DidOpenTextDocumentParams>(j);
  } else if (method == "textDocument/didChange") {
    return as_notification_message<protocol::DidChangeTextDocumentParams>(j);
  } else if (method == "textDocument/didClose") {
    return as_notification_message<protocol::DidCloseTextDocumentParams>(j);
  } else if (method == "textDocument/didSave") {
    return as_notification_message<protocol::DidSaveTextDocumentParams>(j);
    // } else if (method == "textDocument/willSave") {
    //   TODO
  }
}

} // namespace lscpp
