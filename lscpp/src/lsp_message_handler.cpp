/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "lsp_message_handler.h"

#include "../external/json.hpp"
#include "protocol_serializer/serializer.h"
#include <loguru.hpp>

namespace lscpp {

namespace {
protocol::InitializeParams parse_initialize_params(nlohmann::json const &j) {
  return j.get<protocol::InitializeParams>();
}

template <typename Result>
nlohmann::json make_response_message(int id, Result const &result) {
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
} // namespace

std::optional<std::string>
lsp_message_handler::handle_message(lsp_server &server,
                                    std::string const &request) {
  nlohmann::json j = nlohmann::json::parse(request);

  if (!initialized_) {
    if (j["method"] != "initialize") {
      LOG_F(INFO, "Expected initialize request");
      exit(1);
    } else {
      initialized_ = true;
      auto init_params = parse_initialize_params(j["params"]);
      auto init_result = server.initialize(init_params);
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
  } else if (shutdown_) {
    if (j["method"] == "exit") {
      exit(0); // TODO give the user the chance to do something, i.e. call a
               // virtual method
    } else {
      // TODO: send invalid request
      exit(1);
    }
  } else {
    if (j["method"] == "shutdown") {
      shutdown_ = true;
      nlohmann::json j_null;
      auto json_result = make_response_message(j["id"], j_null);
      return json_result.dump();
    } else if (j["method"] == "textDocument/hover") {
      LOG_F(INFO, "Received textDocument/hover");
      protocol::TextDocumentPositionParams params;
      j.at("params").get_to(params);
      auto result = server.getTextDocumentService().hover(params);
      auto json_result = make_response_message(j["id"], result);
      return json_result.dump();
    } else if (j["method"] == "textDocument/completion") {
      LOG_F(INFO, "Received textDocument/completion");
      protocol::CompletionParams params{};
      j.at("params").get_to(params);
      auto result = server.getTextDocumentService().completion(params);
      auto json_result = make_response_message(j["id"], result);
      return json_result.dump();
    } else if (j["method"] == "textDocument/definition") {
      LOG_F(INFO, "Received textDocument/definition");
      protocol::TextDocumentPositionParams params{};
      j.at("params").get_to(params);
      auto result = server.getTextDocumentService().definition(params);
      auto json_result = make_response_message(j["id"], result);
      return json_result.dump();
    } else if (j["method"] == "textDocument/didOpen") {
      LOG_F(INFO, "Received textDocument/didOpen");
      protocol::DidOpenTextDocumentParams params{};
      j.at("params").get_to(params);
      server.getTextDocumentService().didOpen(params);
    } else if (j["method"] == "textDocument/didChange") {
      LOG_F(INFO, "Received textDocument/didChange");
      protocol::DidChangeTextDocumentParams params{};
      j.at("params").get_to(params);
      server.getTextDocumentService().didChange(params);
    } else if (j["method"] == "textDocument/didClose") {
      LOG_F(INFO, "Received textDocument/didClose");
      protocol::DidCloseTextDocumentParams params{};
      j.at("params").get_to(params);
      server.getTextDocumentService().didClose(params);
    } else if (j["method"] == "textDocument/didSave") {
      LOG_F(INFO, "Received textDocument/didSave");
      protocol::DidSaveTextDocumentParams params{};
      j.at("params").get_to(params);
      server.getTextDocumentService().didSave(params);
    } else if (j["method"] == "textDocument/willSave") {
      LOG_F(INFO, "Received textDocument/willSave");
    }
  }
  return std::nullopt;
}

} // namespace lscpp
