#include "listfile_parser.hpp"
#include "listfile_query.hpp"
#include <cmListFileCache.h> //TODO remove dependency

#include <lsp_launcher.h>
#include <lsp_server.h>
#include <map>
#include <stdexcept>

using namespace lscpp; // TODO remove

class cmakels : public lscpp::lsp_server, lscpp::TextDocumentService {
private:
  std::map<std::string, std::optional<cmListFile>> open_files;

public:
  protocol::InitializeResult
  initialize(const protocol::InitializeParams &params) override {
    protocol::ServerCapabilities capabilites;
    capabilites.hoverProvider = true;
    capabilites.completionProvider = protocol::CompletionOptions{};
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
    if (!open_files[position.textDocument.uri])
      return {"Parsing Error"};
    try {
      auto result = cmake_query::get_function(
          *(open_files[position.textDocument.uri]),
          {static_cast<std::size_t>(position.position.line),
           static_cast<std::size_t>(position.position.character)});
      return {cmake_query::get_selected_token(result)};
    } catch (std::runtime_error e) {
      return {"No result"};
    }
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
    // TODO don't parse everytime: probably save the string and keep a cache and
    // keep a cache for parsed file
  }
  void didClose(protocol::DidCloseTextDocumentParams params) override {
    open_files.erase(params.textDocument.uri);
  }
};

int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  loguru::init(argc, argv);
  loguru::add_file("sample_server.log", loguru::Truncate,
                   loguru::Verbosity_MAX);

  lscpp::lsp_launcher launcher{std::make_unique<cmakels>()};
  launcher.start();
}
