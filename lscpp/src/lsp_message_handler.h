#pragma once

#include "../external/json.hpp"
#include "lsp_server.h"
#include <loguru.hpp>

#include "protocol_serializer/serializer.h"

namespace lscpp {

protocol::InitializeParams parse_initialize_params(nlohmann::json const &j) {
  return j.get<protocol::InitializeParams>();
}

class lsp_message_handler {
  lsp_server &server_;
  bool initialized_ = false;
  bool ready_ = false; // after received initialize

public:
  lsp_message_handler(lsp_server &server) : server_{server} {}

  template <typename Result>
  nlohmann::json make_response_message(int id, Result const &result) {
    // nlohmann::json j_result = result;
    nlohmann::json j{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
    return j;
  }

  nlohmann::json make_notification_message(std::string const &msg) {
    nlohmann::json j{{"jsonrpc", "2.0"}, {"method", "window/showMessage"}};
    nlohmann::json params{};
    params.emplace("type", 3);
    params.emplace("message", msg);
    j.emplace("params", params);
    //  {"parameters", {"type", "3"}, {"message", msg}} };
    return j;
  }

  std::string handle_request(std::string const &request) {
    nlohmann::json j = nlohmann::json::parse(request);

    if (!initialized_) {
      if (j["method"] != "initialize") {
        LOG_F(INFO, "Expected initialize request");
        exit(1);
      } else {
        initialized_ = true;
        auto init_params = parse_initialize_params(j["params"]);
        auto init_result = server_.initialize(init_params);
        // nlohmann::json json_result = init_result.json();
        auto json_result = make_response_message(j["id"], init_result);
        return json_result.dump();
      }
    } else if (!ready_) {
      if (j["method"] != "initialized") {
        LOG_F(INFO, "Expected initialized request");
        exit(1);
      } else {
        ready_ = true;
        return make_notification_message("I got initialized!").dump();
        // auto init_params = parse_initialize_params(j["params"]);
        // auto init_result = server_.initialize(init_params);
        // // nlohmann::json json_result = init_result.json();
        // auto json_result = make_response_message(j["id"], init_result);
        // return json_result.dump();
      }
    } else {
      if (j["method"] == "textDocument/hover") {
        protocol::TextDocumentPositionParams params{};
        auto result = server_.getTextDocumentService().hover(params);
        auto json_result = make_response_message(j["id"], result);
        return json_result.dump();
        // return make_response_message(j["id"], const Result &result)
        // return make_notification_message("some request").dump();
      }
    }
    return {};
  }
};

} // namespace lscpp
