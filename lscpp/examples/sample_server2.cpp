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
  protocol::InitializeResult
  initialize(const protocol::InitializeParams &params) override {
    protocol::ServerCapabilities capabilites;
    capabilites.hoverProvider = true;
    protocol::InitializeResult result{capabilites};
    return result;
  }

  TextDocumentService &getTextDocumentService() override { return *this; }

  protocol::Hover
  hover(protocol::TextDocumentPositionParams position) override {
    return {"Blablabla!"};
  }
};

} // namespace lscpp

int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  loguru::init(argc, argv);
  loguru::add_file("/home/vogtha/tmp/sample_server.log", loguru::Truncate,
                   loguru::Verbosity_MAX);

  lscpp::lsp_launcher launcher{std::make_unique<lscpp::my_lsp_server>()};
  launcher.start();
}
