#pragma once

#include "protocol/CompletionItem.h"
#include "protocol/CompletionParams.h"
#include "protocol/Hover.h"
#include "protocol/InitializeParams.h"
#include "protocol/InitializeResult.h"
#include "protocol/TextDocumentPositionParams.h"
#include <variant>

namespace lscpp {

class TextDocumentService {
public:
  virtual protocol::Hover
  hover(protocol::TextDocumentPositionParams position) = 0;

  virtual std::variant<
      std::vector<protocol::CompletionItem> /* , CompletionList */> // TODO
  completion(protocol::CompletionParams params) = 0;
};

class lsp_server {
public:
  virtual protocol::InitializeResult
  initialize(protocol ::InitializeParams const &params) = 0;

  virtual TextDocumentService &getTextDocumentService() = 0;

  virtual ~lsp_server() {}
};

} // namespace lscpp
