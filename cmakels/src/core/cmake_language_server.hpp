/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <cmListFileCache.h> //TODO remove dependency

#include "../cmake_query/cmake_query.hpp"
#include <lscpp/lsp_server.h>
#include <lscpp/protocol/extensions/Dependencies.h>
#include <map>
#include <optional>
#include <string>

namespace cmakels {

class cmake_language_server : public lscpp::lsp_server,
                              lscpp::TextDocumentService,
                              lscpp::CustomMessageService {
private:
  std::string root_path_;
  std::string build_directory_;
  std::map<std::string, std::optional<cmListFile>> open_files_;
  std::optional<cmake_query::cmake_query> query_;

public:
  cmake_language_server(std::string build_directory)
      : build_directory_{build_directory} {}

  lscpp::protocol::InitializeResult
  initialize(const lscpp::protocol::InitializeParams &params) override;
  TextDocumentService &getTextDocumentService() override;
  CustomMessageService &getCustomMessageService() override;

  lscpp::protocol::Hover
  hover(lscpp::protocol::TextDocumentPositionParams position) override;

  lscpp::protocol::Location
  definition(lscpp::protocol::TextDocumentPositionParams position) override;

  std::variant<std::vector<lscpp::protocol::CompletionItem>>
  completion(lscpp::protocol::CompletionParams params) override;

  void didOpen(lscpp::protocol::DidOpenTextDocumentParams params) override;

  void didChange(lscpp::protocol::DidChangeTextDocumentParams params) override;

  void didClose(lscpp::protocol::DidCloseTextDocumentParams params) override;

  void didSave(lscpp::protocol::DidSaveTextDocumentParams params) override;

  lscpp::protocol::extensions::Dependencies
  dependencies(lscpp::protocol::TextDocumentPositionParams position) override;
};
} // namespace cmakels
