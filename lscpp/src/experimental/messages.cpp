#include "../external/json.hpp"
#include "../protocol_serializer/serializer.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include <lscpp/experimental/messages.h>

namespace lscpp {

namespace {
template <typename Result>
nlohmann::json make_response_message(int id, Result const &result) {
  nlohmann::json j{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
  return j.dump();
}
} // namespace

std::string notification_message(std::string const &msg) {
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

request parse_request(std::string request) {
  nlohmann::json j = nlohmann::json::parse(request);
  auto const &method = j["method"];
  auto const &params = j["params"];
  int id = j["id"];

  if (method == "initialize") {
    return {method, id, params.get<protocol::InitializeParams>()};
  } else if (method == "initialized") {
    return {method, id};
  } else if (method == "exit") {
    return {method, id};
  } else if (method == "shutdown") {
    return {method, id};
  } else if (method == "textDocument/hover") {
    return {method, id, params.get<protocol::TextDocumentPositionParams>()};
  }
  // TODO error
}

} // namespace lscpp
