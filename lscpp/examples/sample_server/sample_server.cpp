/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <loguru.hpp>

#include <lscpp/lsp_launcher.h>
#include <lscpp/lsp_server.h>

namespace lscpp {

class my_lsp_server : public lsp_server, TextDocumentService {
private:
  int counter;

public:
  protocol::InitializeResult
  initialize(const protocol::InitializeParams &params) override {
    protocol::ServerCapabilities capabilites;
    capabilites.hoverProvider = true;
    capabilites.completionProvider = protocol::CompletionOptions{};
    capabilites.definitionProvider = true;
    protocol::TextDocumentSyncOptions sync;
    sync.openClose = true;
    sync.change = protocol::TextDocumentSyncKind::Full;
    capabilites.textDocumentSync = sync;
    protocol::InitializeResult result{capabilites};
    return result;
  }

  TextDocumentService &getTextDocumentService() override { return *this; }

  protocol::Hover
  hover(protocol::TextDocumentPositionParams position) override {
    return {"Blablabla!" + position.textDocument.uri};
  }

  protocol::Location
  definition(protocol::TextDocumentPositionParams position) override {
    return {position.textDocument.uri, {{0, 0}, {0, 0}}};
  }

  std::variant<std::vector<protocol::CompletionItem>>
  completion(protocol::CompletionParams params) override {
    LOG_F(INFO, "In sample_server2: completion");
    counter++;
    return std::vector<protocol::CompletionItem>{
        {"bla" + params.textDocument.uri}};
  }

  void didOpen(protocol::DidOpenTextDocumentParams params) override {
    LOG_F(INFO, "Opening: %s", params.textDocument.uri.c_str());
  }

  void didChange(protocol::DidChangeTextDocumentParams params) override {
    LOG_F(INFO, "Changing: %s", params.textDocument.uri.c_str());
  }

  void didClose(protocol::DidCloseTextDocumentParams params) override {
    LOG_F(INFO, "Closing: %s", params.textDocument.uri.c_str());
  }

  void didSave(protocol::DidSaveTextDocumentParams params) override {
    LOG_F(INFO, "Saving: %s", params.textDocument.uri.c_str());
  }
};

} // namespace lscpp

int main(int argc, char *argv[]) {
  lscpp::launch(
      lscpp::my_lsp_server{},
      lscpp::launch_config{0, {lscpp::Verbosity_MAX, "sample_server.log"}},
      lscpp::stdio_transporter{false});
}
