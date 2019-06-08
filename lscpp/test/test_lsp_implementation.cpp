#include "../external/json.hpp"
#include <any>
#include <gtest/gtest.h>
#include <sys/wait.h>
#include <variant>

class Message {
  const std::string jsonrpc = "2.0";

  friend void to_json(nlohmann::json &j, const Message &m) {
    j = nlohmann::json{{"jsonrpc", m.jsonrpc}};
  }
};

class RequestMessage : public Message {
  /**
   * The request id.
   */
  std::variant<int, std::string> id;

  /**
   * The method to be invoked.
   */
  std::string method;

  /**
   * The method's params.
   */
  std::any params; // Array<any> | object
};

class ServerCapabilities {
  bool hoverProvider_;

public:
  void setHoverProvider(bool hoverProvider) { hoverProvider_ = hoverProvider; }
};

struct InitializeResult {
  ServerCapabilities capabilities;
};

class DocumentUri {
  // TODO
};

class WorkspaceFolder {
  // TODO
};

class WorkspaceClientCapabilities {
  // TODO
};

class TextDocumentClientCapabilities {
  // TODO
};

class ClientCapabilities {
  /**
   * Workspace specific client capabilities.
   */
  std::optional<WorkspaceClientCapabilities> workspace;

  /**
   * Text document specific client capabilities.
   */
  std::optional<TextDocumentClientCapabilities> textDocument;

  /**
   * Experimental client capabilities.
   */
  std::any experimental;
};

class InitializeParams {
  /**
   * The process Id of the parent process that started
   * the server. Is null if the process has not been started by another process.
   * If the parent process is not alive then the server should exit (see exit
   * notification) its process.
   */
  std::optional<int> processId;

  /**
   * The rootPath of the workspace. Is null
   * if no folder is open.
   *
   * @deprecated in favour of rootUri.
   */
  std::optional<std::string> rootPath;

  /**
   * The rootUri of the workspace. Is null if no
   * folder is open. If both `rootPath` and `rootUri` are set
   * `rootUri` wins.
   */
  std::optional<DocumentUri> rootUri;

  /**
   * User provided initialization options.
   */
  std::any initializationOptions;

  /**
   * The capabilities provided by the client (editor or tool)
   */
  ClientCapabilities capabilities;

  /**
   * The initial trace setting. If omitted trace is disabled ('off').
   */
  std::string trace = "off"; // 'off' | 'messages' | 'verbose'; TODO maybe enum

  /**
   * The workspace folders configured in the client when the server starts.
   * This property is only available if the client supports workspace folders.
   * It can be `null` if the client supports workspace folders but none are
   * configured.
   *
   * Since 3.6.0
   */
  std::vector<WorkspaceFolder> workspaceFolders;
};

class lsp_server {
  virtual InitializeResult initialize(InitializeParams const &params) = 0;

  // virtual void get_text_document_service() = 0;
};

class my_lsp_server : public lsp_server {
  InitializeResult initialize(const InitializeParams &params) override {
    ServerCapabilities capabilites;
    capabilites.setHoverProvider(true);
    InitializeResult result{capabilites};
    return result;
  }
};

template <typename Server> class LSPLauncher {
  Server server_;

  LSPLauncher(Server &&server) : server_(server) {}

  void start() {}
};

TEST(json, simple) { my_lsp_server server; }
