// #include "../external/json.hpp"
#include "lscpp/lsp_header.h" //TODO
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include "lscpp/transporter.h"
#include "messages.h"
#include <chrono>
#include <functional>
#include <future>
#include <sstream>
#include <string>
#include <thread>

namespace lscpp::experimental {
namespace prot = lscpp::protocol;
namespace {
// prot::InitializeParams parse_initialize_params(nlohmann::json const &j) {
//   return j.get<prot::InitializeParams>();
// }

// template <typename Result>
// nlohmann::json make_response_message(int id, Result const &result) {
//   nlohmann::json j{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
//   return j;
// }

} // namespace

struct not_implemented {};

struct lscpp_message_handler {
  bool initialized_ = false;
  bool ready_ = false;
  bool shutdown_ = false;
};

template <class Server>
prot::InitializeResult
lscpp_handle_initialize(Server &server,
                        prot::InitializeParams const &init_params);

// default
template <class Server>
prot::InitializeResult
lscpp_handle_request_initialize(Server &server,
                                prot::InitializeParams const &init_params) {

  return lscpp_handle_initialize(server, init_params);
}

not_implemented lscpp_get_message_handler(...);

not_implemented lscpp_handle_hover(...);

// template <class Server,
//           class Res = decltype(lscpp_handle_hover(
//               std::declval<Server &>(),
//               std::declval<prot::TextDocumentPositionParams const &>()))>
// std::enable_if_t<std::is_same_v<Res, not_implemented>, prot::Hover>
// lscpp_handle_hover_dispatch(Server &server,
//                             prot::TextDocumentPositionParams params) {
//   static_assert(sizeof(Server) < 0, "hover not implemented");
// }

template <class Server>
constexpr bool has_hover = !std::is_same_v<
    decltype(lscpp_handle_hover(
        std::declval<Server &>(),
        std::declval<prot::TextDocumentPositionParams const &>())),
    not_implemented>;

// template <class Server>
// auto lscpp_handle_hover_dispatch(Server &server,
//                                  prot::TextDocumentPositionParams params) {
//   return lscpp_handle_hover(server, params);
// }
// template <class Server,
//           class Res = decltype(lscpp_handle_hover(
//               std::declval<Server &>(),
//               std::declval<prot::TextDocumentPositionParams const &>()))>
// std::enable_if_t<!std::is_same_v<Res, not_implemented>, Res>
// lscpp_handle_hover_dispatch(Server &server,
//                             prot::TextDocumentPositionParams params) {
//   return lscpp_handle_hover(server, params);
// }

template <class Server>
std::optional<std::string> lscpp_handle_message(lscpp_message_handler &hndlr,
                                                Server &server,
                                                std::string const &request) {
  // nlohmann::json j = nlohmann::json::parse(request);
  auto r = parse_request(request);
  auto method = r.type;
  auto id = r.id;
  if (!hndlr.initialized_) {
    if (method != "initialize") {
      //   LOG_F(INFO, "Expected initialize request");
      exit(1);
    } else {
      auto init_result = lscpp_handle_initialize(
          server, std::any_cast<protocol::InitializeParams>(r.data));
      return initialize_response(id, init_result);
      // hndlr.initialized_ = true;
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
      return notification_message("I got initialized!");
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
      return shutdown_response(id);
    } else if (method == "textDocument/hover") {
      if constexpr (has_hover<Server>) {
        //   LOG_F(INFO, "Received textDocument/hover");
        // protocol::TextDocumentPositionParams params;
        // j.at("params").get_to(params);

        auto result = lscpp_handle_hover(
            server,
            std::any_cast<protocol::TextDocumentPositionParams>(r.data));
        return hover_response(id, result);
      } else {
        exit(1);
      }
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
  //   loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
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
