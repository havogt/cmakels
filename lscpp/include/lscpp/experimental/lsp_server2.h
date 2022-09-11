// #include "../external/json.hpp"
#include "lscpp/lsp_header.h" //TODO
#include "lscpp/protocol/DidChangeTextDocumentParams.h"
#include "lscpp/protocol/DidCloseTextDocumentParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/DidSaveTextDocumentParams.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include "lscpp/transporter.h"
#include "messages.h"
#include <chrono>
#include <functional>
#include <future>
#include <iostream> // TODO remove
#include <sstream>
#include <string>
#include <thread>

namespace lscpp::experimental {

struct not_implemented {};

struct lscpp_message_handler {
  bool initialized_ = false;
  bool ready_ = false;
  bool shutdown_ = false;
};

// template <class Server>
// protocol::InitializeResult
// lscpp_handle_initialize(Server &server,
//                         protocol::InitializeParams const &init_params);

not_implemented lscpp_get_message_handler(...);

not_implemented lscpp_handle_initialize(...);
not_implemented lscpp_handle_hover(...);

not_implemented lscpp_handle_did_open(...);
not_implemented lscpp_handle_did_change(...);
not_implemented lscpp_handle_did_close(...);

template <class Server>
constexpr bool has_hover =
    !std::is_same_v<decltype(lscpp_handle_hover(
                        std::declval<Server &>(),
                        std::declval<protocol::TextDocumentPositionParams>())),
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
protocol::InitializeResult lscpp_handle_initialize_default(
    Server &server_, lscpp::protocol::InitializeParams const &init_params) {

  using namespace lscpp;
  protocol::ServerCapabilities capabilites;
  capabilites.hoverProvider = experimental::has_hover<Server>;
  capabilites.completionProvider = protocol::CompletionOptions{};
  capabilites.definitionProvider = false;
  protocol::TextDocumentSyncOptions sync;
  sync.openClose = experimental::has_text_document_sync<Server>;
  sync.change = protocol::TextDocumentSyncKind::Full;
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

template <class Server>
std::optional<std::string> lscpp_handle_message(lscpp_message_handler &hndlr,
                                                Server &server,
                                                std::string const &request) {
  auto message = parse_request(request);
  auto method = message.method;

  if (!hndlr.initialized_) {
    if (method != "initialize") {
      //   LOG_F(INFO, "Expected initialize request");
      exit(1);
    } else {
      auto r = std::get<request_message>(message.data);
      auto id = r.id;
      auto init_result = handle_initialize(
          server, std::any_cast<protocol::InitializeParams>(r.params));
      hndlr.initialized_ = true;
      return initialize_response(id, init_result);
      // auto init_params = parse_initialize_params(j["params"]);
      // // nlohmann::json json_result = init_result.json();
      // auto json_result = make_response_message(j["id"], init_result);
      // return json_result.dump();
    }
  } else if (!hndlr.ready_) {
    if (method != "initialized") {
      //   LOG_F(INFO, "Expected initialized request");
      exit(1);
    } else {
      hndlr.ready_ = true;
      return make_notification_message("I got initialized!");
      // auto init_params = parse_initialize_params(j["params"]);
      // auto init_result = server_.initialize(init_params);
      // // nlohmann::json json_result = init_result.json();
      // auto json_result = make_response_message(j["id"], init_result);
      // return json_result.dump();
    }
  } else if (hndlr.shutdown_) {
    if (method == "exit") {
      exit(0); // TODO give the user the chance to do something, i.e. call a
               // virtual method
    } else {
      // TODO: send invalid request
      exit(1);
    }
  } else {
    if (method == "shutdown") {
      hndlr.shutdown_ = true;
      auto r = std::get<request_message>(message.data);
      return shutdown_response(r.id);
    } else if (method == "textDocument/hover") {
      if constexpr (has_hover<Server>) {
        auto r = std::get<request_message>(message.data);
        auto id = r.id;
        auto result = lscpp_handle_hover(
            server,
            std::any_cast<protocol::TextDocumentPositionParams>(r.params));
        return hover_response(id, result);
      } else {
        exit(1);
      }
    } else if (method == "textDocument/didOpen") {
      auto r = std::get<notification_message>(message.data);
      auto params =
          std::any_cast<protocol::DidOpenTextDocumentParams>(r.params);
      lscpp_handle_did_open(server, params);
      return {};
    } else if (method == "textDocument/didChange") {
      lscpp_handle_did_change(
          server, std::any_cast<protocol::DidChangeTextDocumentParams>(
                      std::get<notification_message>(message.data).params));
      return {};
    } else if (method == "textDocument/didClose") {
      lscpp_handle_did_close(
          server, std::any_cast<protocol::DidCloseTextDocumentParams>(
                      std::get<notification_message>(message.data).params));
      return {};
    } else if (method == "textDocument/didSave") {
      lscpp_handle_did_save(
          server, std::any_cast<protocol::DidSaveTextDocumentParams>(
                      std::get<notification_message>(message.data).params));
      return {};
    }
    exit(1);
  }
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

inline void write_lsp_message(transporter &t, std::string const &content) {
  std::stringstream content_length;
  content_length << "Content-Length: ";
  content_length << content.size();
  t.write_message(content_length.str(), true);
  t.write_message("", true);

  t.write_message(content);
}

template <class Server>
void launch(Server &&server, transporter &&transporter_) {

  // Allows to attach a debugger,
  // before the language server starts to communicate with the client.
  //   std::this_thread::sleep_for(std::chrono::seconds(config.startup_delay));

  // If we want to play with different message handlers, we can revert to a
  // templated launch (or duplicate the launch as long as it is simple)
  //   lsp_message_handler message_handler{};
  //   auto &message_handler = lscpp_get_message_handler(server);

  // setup logger
  // loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  //   loguru::g_colorlogtostderr = false;
  //   if (config.logger.filename.size() > 0)
  //     loguru::add_file(config.logger.filename.c_str(), loguru::Truncate,
  //                      config.logger.verbosity);

  auto rcv = std::async(std::launch::async, [&]() {
    while (true) {
      auto header = parse_header(transporter_);
      //   LOG_F(INFO, "content-length: '%d'", header.content_length);
      if (header.content_length <
          0) { // TODO parse header should not use -1 to report an error
        continue;
      }
      auto msg = transporter_.read_message(header.content_length);
      //   LOG_F(INFO, "msg: '%s'", msg.c_str());

      //   auto result = message_handler.handle_message(server, msg);
      auto result = lscpp_handle_message(server, msg);
      if (result) {
        // LOG_F(INFO, "Sending response: '%s'", (*result).c_str());
        write_lsp_message(transporter_, *result);
      }
    }
  });

  rcv.wait();
}

} // namespace lscpp::experimental
