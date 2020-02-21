/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cmake_language_server.hpp"

#include "../listfile_parser/listfile_parser.hpp"
#include "../listfile_parser/listfile_query.hpp"
#include "../support/filesystem.hpp"
#include "../support/find_replace.hpp"
#include "../support/uri_encode.hpp"
#include <cstdint>
#include <iostream>
#include <regex>
#include <stdexcept>

using namespace lscpp;            // TODO remove
using namespace cmakels::support; // TODO remove
using namespace cmakels::parser;  // TODO remove

namespace cmakels {

namespace {
std::string substitute_variables(std::string const &token, std::string uri,
                                 cmakels::cmake_query::cmake_query &query) {
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
} // namespace

protocol::InitializeResult
cmake_language_server::initialize(const protocol::InitializeParams &params) {
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

TextDocumentService &cmake_language_server::getTextDocumentService() {
  return *this;
}

protocol::Hover
cmake_language_server::hover(protocol::TextDocumentPositionParams position) {
  if (!open_files_[position.textDocument.uri])
    return {"Parsing Error"};
  try {
    auto result = parser::get_function(
        *(open_files_[position.textDocument.uri]),
        {static_cast<std::size_t>(position.position.line),
         static_cast<std::size_t>(position.position.character)});
    auto selected_token = parser::get_selected_token(result);
    auto ret = substitute_variables(selected_token, position.textDocument.uri,
                                    *query_);
    if (auto target_location = query_->target_definition_location(
            ret, position.textDocument.uri)) {
      auto srcs = query_->get_target_sources(ret, position.textDocument.uri);
      std::string target_string = "Target *" + ret + "*";
      if (srcs) {
        target_string += "\n\nSources:";
        for (auto const &src : *srcs) {
          target_string += "\n\n" + src;
        }
      }
      return {target_string};
    }
    return {ret};
  } catch (std::runtime_error e) { // TODO fix this exception pattern
    return {"No result: " + std::string{e.what()}};
  }
}

protocol::Location cmake_language_server::definition(
    protocol::TextDocumentPositionParams position) {
  auto result = parser::get_function(
      *(open_files_[position.textDocument.uri]),
      {static_cast<std::size_t>(position.position.line),
       static_cast<std::size_t>(position.position.character)});
  if (result.function.Name.Lower.compare("add_subdirectory") == 0) {
    fs::path p = position.textDocument.uri;
    p.remove_filename();
    return {(p / parser::get_name(parser::get_arguments(result.function)[0]) /
             "CMakeLists.txt")
                .string(),
            {{0, 0}, {0, 0}}};
  }
  auto evaluated_selected_token = substitute_variables(
      parser::get_selected_token(result), position.textDocument.uri, *query_);
  if (auto target_location = query_->target_definition_location(
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
        fs::path{uri_to_filename(position.textDocument.uri)}.remove_filename();
    auto maybe_absolute_path = current_source_dir / maybe_path;
    std::cerr << maybe_absolute_path.string() << std::endl;
    if (fs::is_regular_file(maybe_absolute_path)) {
      return {filename_to_uri(maybe_absolute_path.string()), {}};
    }
  }
  return {position.textDocument.uri, {{0, 0}, {0, 0}}};
}

std::variant<std::vector<protocol::CompletionItem>>
cmake_language_server::completion(protocol::CompletionParams params) {
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

void cmake_language_server::didOpen(
    protocol::DidOpenTextDocumentParams params) {
  open_files_.emplace(
      std::make_pair(params.textDocument.uri,
                     parser::parse_listfile(params.textDocument.text)));
}
void cmake_language_server::didChange(
    protocol::DidChangeTextDocumentParams params) {
  open_files_[params.textDocument.uri] =
      parser::parse_listfile(params.contentChanges[0].text); // using full sync
  // TODO don't parse everytime: probably save the string and keep a cache
  // and keep a cache for parsed file
}
void cmake_language_server::didClose(
    protocol::DidCloseTextDocumentParams params) {
  open_files_.erase(params.textDocument.uri);
}

void cmake_language_server::didSave(
    protocol::DidSaveTextDocumentParams params) {
  query_->configure();
}
} // namespace cmakels
