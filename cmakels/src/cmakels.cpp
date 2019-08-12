#include "cmake_query.hpp" // TODO refactor
#include "listfile_parser.hpp"
#include "listfile_query.hpp"
#include "loguru.hpp"
#include <cmListFileCache.h> //TODO remove dependency

#include <experimental/filesystem>
#include <lsp_launcher.h>
#include <lsp_server.h>
#include <map>
#include <regex>
#include <stdexcept>

using namespace lscpp; // TODO remove

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
  std::string root_path;
  std::string build_directory;
  std::map<std::string, std::optional<cmListFile>> open_files;
  cmake_query::cmake_query query_;

public:
  cmakels(std::string root_path, std::string build_directory)
      : root_path{root_path}, build_directory{build_directory},
        query_(root_path, build_directory) {
    query_.configure();
  }

  protocol::InitializeResult
  initialize(const protocol::InitializeParams &params) override {
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
    if (!open_files[position.textDocument.uri])
      return {"Parsing Error"};
    try {
      auto result = cmake_query::get_function(
          *(open_files[position.textDocument.uri]),
          {static_cast<std::size_t>(position.position.line),
           static_cast<std::size_t>(position.position.character)});
      auto ret = substitute_variables(cmake_query::get_selected_token(result),
                                      position.textDocument.uri, query_);
      if (query_.is_target(ret, position.textDocument.uri)) {
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
        *(open_files[position.textDocument.uri]),
        {static_cast<std::size_t>(position.position.line),
         static_cast<std::size_t>(position.position.character)});
    if (result.function.Name.Lower.compare("add_subdirectory") == 0) {
      namespace fs = std::experimental::filesystem;
      fs::path p = position.textDocument.uri;
      p.remove_filename();
      return {(p /
               cmake_query::get_name(
                   cmake_query::get_arguments(result.function)[0]) /
               "CMakeLists.txt")
                  .c_str(),
              {{0, 0}, {0, 0}}};
    }
    return {position.textDocument.uri, {{0, 0}, {0, 0}}};
  }

  std::variant<std::vector<protocol::CompletionItem>>
  completion(protocol::CompletionParams params) override {
    LOG_F(INFO, "In sample_server2: completion");
    return std::vector<protocol::CompletionItem>{
        {"bla" + params.textDocument.uri}};
  }

  void didOpen(protocol::DidOpenTextDocumentParams params) override {
    open_files.emplace(
        std::make_pair(params.textDocument.uri,
                       cmake_query::parse_listfile(params.textDocument.text)));
  }
  void didChange(protocol::DidChangeTextDocumentParams params) override {
    open_files[params.textDocument.uri] = cmake_query::parse_listfile(
        params.contentChanges[0].text); // using full sync
    // TODO don't parse everytime: probably save the string and keep a cache
    // and keep a cache for parsed file
  }
  void didClose(protocol::DidCloseTextDocumentParams params) override {
    open_files.erase(params.textDocument.uri);
  }

  void didSave(protocol::DidSaveTextDocumentParams params) override {
    query_.configure();
  }
};

std::string uri_to_filename(std::string const &uri) {
  return uri.substr(7, std::string::npos);
}

int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  loguru::init(argc, argv);
  loguru::add_file("sample_server.log", loguru::Append, loguru::Verbosity_MAX);

  lscpp::lsp_launcher launcher{
      std::make_unique<cmakels>(uri_to_filename(argv[1]), argv[2])};
  launcher.start();
}
