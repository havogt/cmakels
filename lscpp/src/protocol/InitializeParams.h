#pragma once

#include <optional>
#include <string>
#include <vector>

#include "DocumentUri.h"

namespace lscpp {
namespace protocol {

struct WorkspaceFolder {
  // TODO
};

struct WorkspaceClientCapabilities {
  // TODO
};

struct TextDocumentClientCapabilities {
  // TODO
};

struct ClientCapabilities {
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
  //   std::any experimental;
};

struct InitializeParams {
  /**
   * The process Id of the parent process that started
   * the server. Is null if the process has not been started by another
   * process. If the parent process is not alive then the server should exit
   * (see exit notification) its process.
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
  //   std::any initializationOptions;

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

} // namespace protocol
} // namespace lscpp
