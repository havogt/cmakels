#include <cstddef>
#include <lscpp/experimental/messages.h>

#include "../external/json.hpp"
#include "../protocol_serializer/serializer.h"
#include "lscpp/protocol/CompletionItem.h"
#include "lscpp/protocol/CompletionParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include <sstream>

namespace lscpp::experimental {

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
  } else if (method == "textDocument/definition") {
    return as_request_message<protocol::TextDocumentPositionParams>(j);
  } else if (method == "textDocument/completion") {
    return as_request_message<protocol::CompletionParams>(j);
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
  assert(false); // unreachable
}

} // namespace lscpp::experimental
