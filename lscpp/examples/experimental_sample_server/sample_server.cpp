/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lscpp/experimental/messages.h>
#include <lscpp/protocol/CompletionItem.h>
#include <lscpp/protocol/CompletionParams.h>
#include <lscpp/protocol/Diagnostic.h>
#include <lscpp/protocol/DidCloseTextDocumentParams.h>
#include <lscpp/protocol/DidOpenTextDocumentParams.h>
#include <lscpp/protocol/DidSaveTextDocumentParams.h>
#include <lscpp/protocol/DocumentDiagnosticParams.h>
#include <lscpp/protocol/DocumentDiagnosticReport.h>
#include <lscpp/protocol/Hover.h>
#include <lscpp/protocol/InitializeParams.h>
#include <lscpp/protocol/Location.h>
#include <lscpp/protocol/Position.h>
#include <lscpp/protocol/Range.h>
#include <lscpp/protocol/TextDocumentPositionParams.h>
#include <lscpp/stdio_transporter.h>

#include <lscpp/experimental/adl_lsp_server.h>

namespace my_lsp_server {
struct server : lscpp::experimental::server_with_default_handler {

  friend lscpp::protocol::Hover
  lscpp_handle_hover(server &, lscpp::protocol::TextDocumentPositionParams) {
    return {"bla", lscpp::protocol::Range{{1, 1}, {1, 5}}};
  }
  friend lscpp::protocol::Location
  lscpp_handle_definition(server &,
                          lscpp::protocol::TextDocumentPositionParams params) {
    return lscpp::protocol::Location{
        params.textDocument.uri,
        lscpp::protocol::Range{lscpp::protocol::Position{1, 1},
                               lscpp::protocol::Position{1, 5}}};
  }
  friend std::vector<lscpp::protocol::CompletionItem>
  lscpp_handle_completion(server &, lscpp::protocol::CompletionParams) {
    return {lscpp::protocol::CompletionItem{"foo"},
            lscpp::protocol::CompletionItem{"bar"}};
  }
  friend lscpp::protocol::RelatedFullDocumentDiagnosticReport
  lscpp_handle_diagnostic_report(server &,
                                 lscpp::protocol::DocumentDiagnosticParams) {
    return {
        "foo",
        {lscpp::protocol::Diagnostic{lscpp::protocol::Range{{2, 1}, {2, 5}},
                                     lscpp::protocol::DiagnosticSeverity::Error,
                                     {},
                                     {},
                                     "Oh that's stupid!"}}};
  }
  friend auto
  lscpp_handle_did_open(server &, lscpp::protocol::DidOpenTextDocumentParams) {
    return lscpp::experimental::make_notification_message("opened");
  }
  friend auto
  lscpp_handle_did_change(server &,
                          lscpp::protocol::DidChangeTextDocumentParams) {
    return lscpp::experimental::make_notification_message("changed");
  }
  friend void
  lscpp_handle_did_close(server &,
                         lscpp::protocol::DidCloseTextDocumentParams) {}
  friend void
  lscpp_handle_did_save(server &, lscpp::protocol::DidSaveTextDocumentParams) {}
};

} // namespace my_lsp_server

int main(int argc, char *argv[]) {
  lscpp::experimental::launch(my_lsp_server::server{},
                              lscpp::stdio_transporter{false});
}
