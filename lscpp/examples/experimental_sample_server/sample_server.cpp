/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
// #include <loguru.hpp>

// #include <lscpp/lsp_launcher.h>
// #include <lscpp/lsp_server.h>

#include "lscpp/protocol/DidCloseTextDocumentParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/DidSaveTextDocumentParams.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"
#include "lscpp/stdio_transporter.h"
#include <lscpp/experimental/lsp_server2.h>

namespace my_lsp_server {
struct server : lscpp::experimental::server_with_default_handler {
  // lscpp::experimental::lscpp_message_handler handler_;

  friend auto lscpp_handle_initialize(
      server &server_, lscpp::protocol::InitializeParams const &init_params) {

    using namespace lscpp;
    protocol::ServerCapabilities capabilites;
    capabilites.hoverProvider = experimental::has_hover<server>;
    capabilites.completionProvider = protocol::CompletionOptions{};
    capabilites.definitionProvider = false;
    protocol::TextDocumentSyncOptions sync;
    sync.openClose = experimental::has_text_document_sync<server>;
    sync.change = protocol::TextDocumentSyncKind::Full;
    capabilites.textDocumentSync = sync;
    protocol::InitializeResult result{capabilites};
    return result;
  }
  // friend auto &lscpp_get_message_handler(server &server_) {
  //   return server_.handler_;
  // }

  friend lscpp::protocol::Hover
  lscpp_handle_hover(server &, lscpp::protocol::TextDocumentPositionParams) {
    return {"bla"};
  }
  friend void
  lscpp_handle_did_open(server &, lscpp::protocol::DidOpenTextDocumentParams) {
    std::cerr << "nothing to do" << std::endl;
  }
  friend void
  lscpp_handle_did_change(server &,
                          lscpp::protocol::DidChangeTextDocumentParams) {}
  friend void
  lscpp_handle_did_close(server &,
                         lscpp::protocol::DidCloseTextDocumentParams) {}
  friend void
  lscpp_handle_did_save(server &, lscpp::protocol::DidSaveTextDocumentParams) {}
};

// class my_lsp_server : public lsp_server, TextDocumentService {
// private:
//   int counter;

// public:
//   protocol::InitializeResult
//   initialize(const protocol::InitializeParams &params) override {
//     protocol::ServerCapabilities capabilites;
//     capabilites.hoverProvider = true;
//     capabilites.completionProvider = protocol::CompletionOptions{};
//     capabilites.definitionProvider = true;
//     protocol::TextDocumentSyncOptions sync;
//     sync.openClose = true;
//     sync.change = protocol::TextDocumentSyncKind::Full;
//     capabilites.textDocumentSync = sync;
//     protocol::InitializeResult result{capabilites};
//     return result;
//   }

//   TextDocumentService &getTextDocumentService() override { return *this; }

//   protocol::Hover
//   hover(protocol::TextDocumentPositionParams position) override {
//     return {"Blablabla!" + position.textDocument.uri};
//   }

//   protocol::Location
//   definition(protocol::TextDocumentPositionParams position) override {
//     return {position.textDocument.uri, {{0, 0}, {0, 0}}};
//   }

//   std::variant<std::vector<protocol::CompletionItem>>
//   completion(protocol::CompletionParams params) override {
//     LOG_F(INFO, "In sample_server2: completion");
//     counter++;
//     return std::vector<protocol::CompletionItem>{
//         {"bla" + params.textDocument.uri}};
//   }

//   void didOpen(protocol::DidOpenTextDocumentParams params) override {
//     LOG_F(INFO, "Opening: %s", params.textDocument.uri.c_str());
//   }

//   void didChange(protocol::DidChangeTextDocumentParams params) override {
//     LOG_F(INFO, "Changing: %s", params.textDocument.uri.c_str());
//   }

//   void didClose(protocol::DidCloseTextDocumentParams params) override {
//     LOG_F(INFO, "Closing: %s", params.textDocument.uri.c_str());
//   }

//   void didSave(protocol::DidSaveTextDocumentParams params) override {
//     LOG_F(INFO, "Saving: %s", params.textDocument.uri.c_str());
//   }
// };

} // namespace my_lsp_server

int main(int argc, char *argv[]) {
  // my_lsp_server::server serv{};

  // using lscpp::experimental::lscpp_get_message_handler;
  // auto hndlr = lscpp_get_message_handler(serv);
  lscpp::experimental::launch(my_lsp_server::server{},
                              lscpp::stdio_transporter{false});
  // lscpp::launch(
  //     lscpp::my_lsp_server{},
  //     lscpp::launch_config{0, {lscpp::Verbosity_MAX, "sample_server.log"}},
  //     lscpp::stdio_transporter{false});
}
