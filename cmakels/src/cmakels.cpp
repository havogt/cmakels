/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cmake_query.hpp" // TODO refactor
#include "listfile_parser.hpp"
#include "listfile_query.hpp"
#include <cmListFileCache.h> //TODO remove dependency

#include "support/filesystem.hpp"
#include <cstdint>
#include <iostream>
#include <lscpp/lsp_launcher.h>
#include <lscpp/lsp_server.h>
#include <map>
#include <regex>
#include <stdexcept>

using namespace lscpp; // TODO remove

std::string uri_to_filename(std::string const &uri) {
  return uri.substr(7, std::string::npos);
}
std::string filename_to_uri(std::string const &filename) {
  return "file://" + filename;
}

std::string substitute_variables(std::string const &token, std::string uri,
                                 cmake_query::cmake_query &query) {
  std::regex var_regex("(\\$\\{(.*?)\\})", std::regex_constants::ECMAScript);
  if (std::regex_search(token, var_regex)) {
    std::string result = token;
    auto words_begin =
        std::sregex_iterator(token.begin(), token.end(), var_regex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
      std::smatch match = *i;
      size_t variable_id = 1;      // matches ${var}
      size_t variable_name_id = 2; // matches var
      std::string match_str = match.str(variable_name_id);
      std::size_t start = result.find(match.str(variable_id));
      result.replace(start, match.length(variable_id),
                     query.evaluate_variable(match_str, uri));
    }
    return result;
  } else {
    return token;
  }
}

class cmakels : public lscpp::lsp_server, lscpp::TextDocumentService {
private:
  std::string root_path_;
  std::string build_directory_;
  std::map<std::string, std::optional<cmListFile>> open_files_;
  std::optional<cmake_query::cmake_query> query_;

public:
  cmakels(std::string build_directory) : build_directory_{build_directory} {}

  protocol::InitializeResult
  initialize(const protocol::InitializeParams &params) override {
    if (auto root_uri = params.rootUri) {
      query_.emplace(uri_to_filename(*(params.rootUri)), build_directory_);
      query_->configure();
    }

    protocol::ServerCapabilities capabilites;
    capabilites.hoverProvider = true;
    capabilites.completionProvider = protocol::CompletionOptions{};
    capabilites.definitionProvider = true;
    protocol::TextDocumentSyncOptions sync;
    sync.openClose = true;
    sync.change = protocol::TextDocumentSyncKind::Full;
    sync.save = {false};
    capabilites.textDocumentSync = sync;
    protocol::InitializeResult result{capabilites};

    return result;
  }

  TextDocumentService &getTextDocumentService() override { return *this; }

  protocol::Hover
  hover(protocol::TextDocumentPositionParams position) override {
    if (!open_files_[position.textDocument.uri])
      return {"Parsing Error"};
    try {
      auto result = cmake_query::get_function(
          *(open_files_[position.textDocument.uri]),
          {static_cast<std::size_t>(position.position.line),
           static_cast<std::size_t>(position.position.character)});
      auto ret = substitute_variables(cmake_query::get_selected_token(result),
                                      position.textDocument.uri, *query_);
      if (auto target_location =
              query_->get_target_info(ret, position.textDocument.uri)) {
        ret += " is a target";
      }
      return {ret};
    } catch (std::runtime_error e) {
      return {"No result: " + std::string{e.what()}};
    }
  }

  protocol::Location
  definition(protocol::TextDocumentPositionParams position) override {
    auto result = cmake_query::get_function(
        *(open_files_[position.textDocument.uri]),
        {static_cast<std::size_t>(position.position.line),
         static_cast<std::size_t>(position.position.character)});
    if (result.function.Name.Lower.compare("add_subdirectory") == 0) {
      fs::path p = position.textDocument.uri;
      p.remove_filename();
      return {(p /
               cmake_query::get_name(
                   cmake_query::get_arguments(result.function)[0]) /
               "CMakeLists.txt")
                  .string(),
              {{0, 0}, {0, 0}}};
    }
    auto evaluated_selected_token =
        substitute_variables(cmake_query::get_selected_token(result),
                             position.textDocument.uri, *query_);
    if (auto target_location = query_->get_target_info(
            evaluated_selected_token, position.textDocument.uri)) {
      return {filename_to_uri(target_location->filename),
              {{static_cast<int>(target_location->line - 1), 0},
               {static_cast<int>(target_location->line - 1), 0}}};
    }
    // heuristic goto file (if a file with exists with the name of the selected
    // token we go to that file)
    std::cerr << evaluated_selected_token << std::endl;
    fs::path maybe_path(evaluated_selected_token);
    std::cerr << maybe_path.string() << std::endl;
    if (maybe_path.is_absolute()) {
      if (fs::is_regular_file(maybe_path)) {
        return {filename_to_uri(maybe_path.string()), {}};
      }
    } else {
      auto current_source_dir =
          fs::path{uri_to_filename(position.textDocument.uri)}
              .remove_filename();
      auto maybe_absolute_path = current_source_dir / maybe_path;
      std::cerr << maybe_absolute_path.string() << std::endl;
      if (fs::is_regular_file(maybe_absolute_path)) {
        return {filename_to_uri(maybe_absolute_path.string()), {}};
      }
    }
    return {position.textDocument.uri, {{0, 0}, {0, 0}}};
  }

  std::variant<std::vector<protocol::CompletionItem>>
  completion(protocol::CompletionParams params) override {
    std::vector<protocol::CompletionItem> result;

    // all files in the same directory (non-filtered)
    auto path =
        fs::path{uri_to_filename(params.textDocument.uri)}.remove_filename();
    for (const auto &entry : fs::directory_iterator(path)) {
      if (fs::is_regular_file(entry))
        result.push_back(
            protocol::CompletionItem{entry.path().filename().string()});
    }

    // all targets in the current mf
    for (const auto &tgt : query_->get_target_names(params.textDocument.uri)) {
      result.push_back(protocol::CompletionItem{tgt});
    }

    return result;
  }

  void didOpen(protocol::DidOpenTextDocumentParams params) override {
    open_files_.emplace(
        std::make_pair(params.textDocument.uri,
                       cmake_query::parse_listfile(params.textDocument.text)));
  }
  void didChange(protocol::DidChangeTextDocumentParams params) override {
    open_files_[params.textDocument.uri] = cmake_query::parse_listfile(
        params.contentChanges[0].text); // using full sync
    // TODO don't parse everytime: probably save the string and keep a cache
    // and keep a cache for parsed file
  }
  void didClose(protocol::DidCloseTextDocumentParams params) override {
    open_files_.erase(params.textDocument.uri);
  }

  void didSave(protocol::DidSaveTextDocumentParams params) override {
    query_->configure();
  }
};

int main(int argc, char *argv[]) {
  // Redirect std::cout to std::cerr to ensure that  cmake doesn't write to
  // stdout and messes up with lsp communication.
  // IMPORTANT: we must never use std::cout for lsp communication!
  std::cout.rdbuf(std::cerr.rdbuf());

  lscpp::launch_config config;
#ifndef NDEBUG
  config.startup_delay = 0;
#endif
  config.logger = {lscpp::Verbosity_MAX, "cmakels.log"};

  if (argc < 2) {
    std::cerr << "Usage " << argv[0] << " <build-dir>\n\n"
              << "<build-dir> - build directory relative to workspace root "
                 "(usually defined by an IDE)"
              << std::endl;
    std::exit(1);
  } else {
    lscpp::launch(cmakels{argv[1]}, config, lscpp::stdio_transporter{false});
  }
}
