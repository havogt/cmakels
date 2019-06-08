#include "../external/json.hpp"
#include <any>
#include <gtest/gtest.h>
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

  friend void to_json(nlohmann::json &j, const RequestMessage &m) {
    j = static_cast<const Message &>(m);
    j.emplace("id", m.id);
    j.emplace("method", m.method);
    nlohmann::json params = m.params->json();
    j.emplace("params", params);
  }
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

class ResponseMessage : public Message {
  /**
   * The request id.
   */
  std::variant<int, std::string> id; // number | string | null

  /**
   * The result of a request. This member is REQUIRED on success.
   * This member MUST NOT exist if there was an error invoking the method.
   */
  std::any result; // string | number | boolean | object | null;

  /**
   * The error object in case a request fails.
   */
  //   error ?: ResponseError<any>; // TODO
};

enum class TextDocumentSyncKind { None };

class TextDocumentSyncOptions {};

class ServerCapabilities {
  /**
   * Defines how text documents are synced. Is either a detailed structure
   * defining each notification or for backwards compatibility the
   * TextDocumentSyncKind number. If omitted it defaults to
   * `TextDocumentSyncKind.None`.
   */
  std::variant<TextDocumentSyncKind, TextDocumentSyncOptions> textDocumentSync;

  /**
   * The server provides hover support.
   */
  bool hoverProvider;
};

void to_json(nlohmann::json &j, const ServerCapabilities &o) {
  //   j = static_cast<const Message&>(m);
  //   j.emplace("id", m.id);
  //   j.emplace("method", m.method);
  //   nlohmann::json params = m.params->json();
  //   j.emplace("params", params);
}

class InitializeResult {
  /**
   * The capabilities the language server provides.
   */
  ServerCapabilities capabilities;
};

void to_json(nlohmann::json &j, const InitializeResult &o) {
  //   j = static_cast<const Message&>(m);
  //   j.emplace("id", m.id);
  //   j.emplace("method", m.method);
  //   nlohmann::json params = m.params->json();
  //   j.emplace("params", params);
}

std::unique_ptr<Message> parseMessage() {
    
}

TEST(json, simple) {
  //  InitializeParams a{1};
  //  auto params = std::make_unique<InitializeParams>(1);
  //  RequestMessage msg{"", 1, "initialize", std::move(params)};
  //  nlohmann::json j = msg;
  //  std::cout << j.dump() << std::endl;

  nlohmann::json from = {{"id", 1},
                         {"jsonrpc", ""},
                         {"method", "initialize"},
                         {"params", {"processId:", 1}}};
  std::cout << from.dump() << std::endl;

//   message_handler hndlr{std::make_unique<my_request_message_handler>()};
//   hndlr.handle(from);

  //  auto req = from.get<RequestMessage>();
  //  nlohmann::json paramsback = req.params->json();
  //  std::cout << paramsback.dump() << std::endl;
}
