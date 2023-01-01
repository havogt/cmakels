/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <cassert>
#include <chrono>
#include <functional>
#include <future>
#include <optional>
#include <sstream>
#include <string>
#include <thread>

#include "lscpp/lsp_header.h"
#include "lscpp/protocol/CompletionParams.h"
#include "lscpp/protocol/DidChangeTextDocumentParams.h"
#include "lscpp/protocol/DidCloseTextDocumentParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/DidSaveTextDocumentParams.h"
#include "lscpp/protocol/DocumentDiagnosticParams.h"
#include "lscpp/protocol/DocumentDiagnosticReport.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/Location.h"
#include "lscpp/protocol/ServerCapabilities.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include "lscpp/transporter.h"
#include "messages.h"
#include "threadsafe_queue.h"
#include <stop_token>

namespace lscpp::experimental {

struct not_implemented {};

struct lscpp_message_handler {
  bool initialized_ = false;
  bool ready_ = false;
  bool shutdown_ = false;
};

not_implemented lscpp_get_message_handler(...);

not_implemented lscpp_handle_initialize(...);
not_implemented lscpp_handle_exit(...);

not_implemented lscpp_handle_hover(...);
not_implemented lscpp_handle_definition(...);
not_implemented lscpp_handle_completion(...);
not_implemented lscpp_handle_diagnostic_report(...);

not_implemented lscpp_handle_did_open(...);
not_implemented lscpp_handle_did_change(...);
not_implemented lscpp_handle_did_close(...);
not_implemented lscpp_handle_did_save(...);

template <class Server>
constexpr bool has_exit =
    !std::is_same_v<decltype(lscpp_handle_exit(std::declval<Server &>())),
                    not_implemented>;

template <class Server>
constexpr bool has_hover =
    !std::is_same_v<decltype(lscpp_handle_hover(
                        std::declval<Server &>(),
                        std::declval<protocol::TextDocumentPositionParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_definition =
    !std::is_same_v<decltype(lscpp_handle_definition(
                        std::declval<Server &>(),
                        std::declval<protocol::TextDocumentPositionParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_completion =
    !std::is_same_v<decltype(lscpp_handle_completion(
                        std::declval<Server &>(),
                        std::declval<protocol::CompletionParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_diagnostic_report =
    !std::is_same_v<decltype(lscpp_handle_diagnostic_report(
                        std::declval<Server &>(),
                        std::declval<protocol::DocumentDiagnosticParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_did_open =
    !std::is_same_v<decltype(lscpp_handle_did_open(
                        std::declval<Server &>(),
                        std::declval<protocol::DidOpenTextDocumentParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_did_change =
    !std::is_same_v<decltype(lscpp_handle_did_change(
                        std::declval<Server &>(),
                        std::declval<protocol::DidChangeTextDocumentParams>())),
                    not_implemented>;

template <class Server>
constexpr bool has_did_close =
    !std::is_same_v<decltype(lscpp_handle_did_close(
                        std::declval<Server &>(),
                        std::declval<protocol::DidCloseTextDocumentParams>())),
                    not_implemented>;

template <class Server> struct has_text_document_sync_impl {
  static_assert((has_did_open<Server> && has_did_change<Server> &&
                 has_did_close<Server>) ||
                    (!has_did_open<Server> && !has_did_change<Server> &&
                     !has_did_close<Server>),
                "Either all or none of `lscpp_handle_did_open`, "
                "`lscpp_handle_did_change` and `lscpp_handle_did_close` need "
                "to be implemented.");
  static constexpr bool value =
      has_did_open<Server> && has_did_change<Server> && has_did_close<Server>;
};

template <class Server>
constexpr bool has_text_document_sync =
    has_text_document_sync_impl<Server>::value;

template <class Server>
constexpr bool has_did_save =
    !std::is_same_v<decltype(lscpp_handle_did_save(
                        std::declval<Server &>(),
                        std::declval<protocol::DidSaveTextDocumentParams>())),
                    not_implemented>;

template <class Server>
protocol::InitializeResult lscpp_handle_initialize_default(
    Server &server_, lscpp::protocol::InitializeParams const &init_params) {

  protocol::ServerCapabilities capabilites;
  capabilites.hoverProvider = has_hover<Server>;
  if (has_completion<Server>) {
    capabilites.completionProvider =
        protocol::CompletionOptions{}; // TODO customizable
  }
  capabilites.definitionProvider = has_definition<Server>;
  if (has_diagnostic_report<Server>) {
    capabilites.diagnosticProvider =
        protocol::DiagnosticOptions{}; // TODO customizable
  }
  protocol::TextDocumentSyncOptions sync;
  sync.openClose = has_text_document_sync<Server>;
  sync.change = protocol::TextDocumentSyncKind::Full; // TODO customizable
  capabilites.textDocumentSync = sync;
  protocol::InitializeResult result{capabilites};
  return result;
}

template <class Server>
std::enable_if_t<
    std::is_same_v<decltype(lscpp_handle_initialize(
                       std::declval<Server &>(),
                       std::declval<protocol::InitializeParams>())),
                   not_implemented>,
    protocol::InitializeResult>
handle_initialize(Server &server,
                  protocol::InitializeParams const &init_params) {

  return lscpp_handle_initialize_default(server, init_params);
}

template <class Server>
std::enable_if_t<
    !std::is_same_v<decltype(lscpp_handle_initialize(
                        std::declval<Server &>(),
                        std::declval<protocol::InitializeParams>())),
                    not_implemented>,
    protocol::InitializeResult>
handle_initialize(Server &server,
                  protocol::InitializeParams const &init_params) {

  return lscpp_handle_initialize(server, init_params);
}

template <method_kind Kind> auto get_params(std::any params) {
  return std::any_cast<to_params_t<Kind>>(params);
}

template <class Server>
std::optional<std::string> lscpp_handle_message(lscpp_message_handler &hndlr,
                                                Server &server,
                                                std::string const &request) {
  auto [method, id, params] = parse_request(request);

  // startup/shutdown phase
  if (!hndlr.initialized_) {
    if (method != method_kind::INITIALIZE) {
      exit(1);
    } else {
      auto init_result = handle_initialize(
          server, get_params<method_kind::INITIALIZE>(params));
      hndlr.initialized_ = true;
      return response_message(id, init_result);
    }
  } else if (!hndlr.ready_) {
    if (method != method_kind::INITIALIZED) {
      exit(1);
    } else {
      hndlr.ready_ = true;
      return make_notification_message("I got initialized!");
    }
  } else if (hndlr.shutdown_) {
    if (method == method_kind::EXIT) {
      if constexpr (has_exit<Server>)
        lscpp_handle_exit();
      exit(0);
    } else {
      // TODO: send invalid request
      exit(1);
    }
  }

  // server is ready and initialized
  switch (method) {
  case method_kind::SHUTDOWN:
    hndlr.shutdown_ = true;
    return response_message(id);
  case method_kind::TEXT_DOCUMENT_HOVER:
    assert(has_hover<Server>);
    if constexpr (has_hover<Server>)
      return response_message(
          id,
          lscpp_handle_hover(
              server, get_params<method_kind::TEXT_DOCUMENT_HOVER>(params)));
  case method_kind::TEXT_DOCUMENT_DEFINITION:
    assert(has_definition<Server>);
    if constexpr (has_definition<Server>)
      return response_message(
          id, lscpp_handle_definition(
                  server,
                  get_params<method_kind::TEXT_DOCUMENT_DEFINITION>(params)));
  case method_kind::TEXT_DOCUMENT_COMPLETION:
    assert(has_completion<Server>);
    if constexpr (has_completion<Server>)
      return response_message(
          id, lscpp_handle_completion(
                  server,
                  get_params<method_kind::TEXT_DOCUMENT_COMPLETION>(params)));
  case method_kind::TEXT_DOCUMENT_DIAGNOSTIC:
    assert(has_diagnostic_report<Server>);
    if constexpr (has_diagnostic_report<Server>) {
      return response_message(
          id, lscpp_handle_diagnostic_report(
                  server,
                  get_params<method_kind::TEXT_DOCUMENT_DIAGNOSTIC>(params)));
    }
  case method_kind::TEXT_DOCUMENT_DID_OPEN:
    assert(has_text_document_sync<Server>);
    if constexpr (has_text_document_sync<Server>) {
      // TODO cleanup dispatching on return type
      // allows to send notifications from the server to the client in a
      // synchronous implementation (e.g. PublishDiagnostics)
      // TODO needs type-checking, currently we blindly forward a json string
      if constexpr (std::is_same_v<
                        std::decay_t<decltype(lscpp_handle_did_open(
                            std::declval<Server>(),
                            std::declval<
                                protocol::DidOpenTextDocumentParams>()))>,
                        std::string>) {
        return lscpp_handle_did_open(
            server, get_params<method_kind::TEXT_DOCUMENT_DID_OPEN>(params));
      } else {
        lscpp_handle_did_open(
            server, get_params<method_kind::TEXT_DOCUMENT_DID_OPEN>(params));
      }
      return {};
    }
  case method_kind::TEXT_DOCUMENT_DID_CHANGE:
    assert(has_text_document_sync<Server>);
    if constexpr (has_text_document_sync<Server>) {
      // TODO see TEXT_DOCUMENT_DID_OPEN
      if constexpr (std::is_same_v<
                        std::decay_t<decltype(lscpp_handle_did_change(
                            std::declval<Server>(),
                            std::declval<
                                protocol::DidChangeTextDocumentParams>()))>,
                        std::string>) {
        return lscpp_handle_did_change(
            server, get_params<method_kind::TEXT_DOCUMENT_DID_CHANGE>(params));
      } else {
        lscpp_handle_did_change(
            server, get_params<method_kind::TEXT_DOCUMENT_DID_CHANGE>(params));
      }
    }
    return {};
  case method_kind::TEXT_DOCUMENT_DID_CLOSE:
    if constexpr (has_text_document_sync<Server>)
      lscpp_handle_did_close(
          server, get_params<method_kind::TEXT_DOCUMENT_DID_CLOSE>(params));
    return {};
  case method_kind::TEXT_DOCUMENT_DID_SAVE:
    assert(has_did_save<Server>);
    if constexpr (has_did_save<Server>) {
      lscpp_handle_did_save(
          server, get_params<method_kind::TEXT_DOCUMENT_DID_SAVE>(params));
    }
    return {};
  case method_kind::INITIALIZE:
  case method_kind::INITIALIZED:
  case method_kind::EXIT:
    exit(1);
  }
  exit(1);
}

template <class Server>
auto lscpp_handle_message(Server &server, std::string const &request) {
  auto &message_handler = lscpp_get_message_handler(server);
  return lscpp_handle_message(message_handler, server, request);
}

class server_with_default_handler {
  lscpp_message_handler handler_;

  friend auto &lscpp_get_message_handler(server_with_default_handler &server_) {
    return server_.handler_;
  }
};

inline void read_message(transporter &transporter_,
                         threadsafe_queue<std::string> &q) {
  while (true) {
    auto header = parse_header(transporter_);

    if (header.content_length <
        0) { // TODO parse header should not use -1 to report an error
      continue;
    }
    auto msg = transporter_.read_message(header.content_length);
    q.push(msg);
  }
}

template <class Server>
void work(std::stop_token t, threadsafe_queue<std::string> &in_queue,
          threadsafe_queue<std::string> &out_queue, Server &server) {
  while (!t.stop_requested()) {
    auto msg = in_queue.wait_and_pop(t);
    if (!msg)
      return;
    auto result = lscpp_handle_message(server, *msg);
    if (result) {
      out_queue.push(*result);
    }
  }
}

inline void write_message(std::stop_token t, transporter &transporter_,
                          threadsafe_queue<std::string> &q) {
  while (!t.stop_requested()) {
    auto msg = q.wait_and_pop(t);
    if (msg)
      write_lsp_message(transporter_, *msg);
  }
}

template <class Server>
void launch(Server &&server, transporter &&transporter_) {
  threadsafe_queue<std::string> in_queue;
  threadsafe_queue<std::string> out_queue;

  std::jthread writer(write_message, std::ref(transporter_),
                      std::ref(out_queue));
  std::jthread worker(work<Server>, std::ref(in_queue), std::ref(out_queue),
                      std::ref(server));
  read_message(transporter_, in_queue);
}

} // namespace lscpp::experimental
