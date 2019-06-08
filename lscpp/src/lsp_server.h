#pragma once

#include "protocol/Hover.h"
#include "protocol/InitializeParams.h"
#include "protocol/InitializeResult.h"
#include "protocol/TextDocumentPositionParams.h"

namespace lscpp {

class TextDocumentService {
public:
  virtual protocol::Hover
  hover(protocol::TextDocumentPositionParams position) = 0;
};

class lsp_server {
public:
  virtual protocol::InitializeResult
  initialize(protocol ::InitializeParams const &params) = 0;

  virtual TextDocumentService &getTextDocumentService() = 0;

  virtual ~lsp_server() {}
};

} // namespace lscpp
