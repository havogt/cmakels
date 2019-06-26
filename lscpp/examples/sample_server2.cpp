#include <any>
#include <future>
#include <loguru.hpp>
#include <optional>
#include <queue>
#include <sstream>
#include <string>

#include <lsp_launcher.h>
#include <lsp_server.h>

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

  std::variant<std::vector<protocol::CompletionItem>>
  completion(protocol::CompletionParams params) override {
    LOG_F(INFO, "In sample_server2: completion");
    counter++;
    return std::vector<protocol::CompletionItem>{
        {"bla" + params.textDocument.uri}};
  }
};

} // namespace lscpp

int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  loguru::init(argc, argv);
  loguru::add_file("sample_server.log", loguru::Truncate,
                   loguru::Verbosity_MAX);

  lscpp::lsp_launcher launcher{std::make_unique<lscpp::my_lsp_server>()};
  launcher.start();
}
