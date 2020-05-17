/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "protocol/CompletionItem.h"
#include "protocol/CompletionParams.h"
#include "protocol/DidChangeTextDocumentParams.h"
#include "protocol/DidCloseTextDocumentParams.h"
#include "protocol/DidOpenTextDocumentParams.h"
#include "protocol/DidSaveTextDocumentParams.h"
#include "protocol/Hover.h"
#include "protocol/InitializeParams.h"
#include "protocol/InitializeResult.h"
#include "protocol/Location.h"
#include "protocol/TextDocumentPositionParams.h"
#include "protocol/extensions/Dependencies.h"
#include <stdexcept>
#include <variant>

namespace lscpp {

/**
 * Language server implementations need to implement the required virtual
 * methods. The respective lsp rpc method names are "textdocument/<methodname>".
 * E.g. a "textdocument/didOpen" notification send by the client will call the
 * didOpen() method in TextDocumentService.
 */
class TextDocumentService {
public:
  virtual protocol::Hover
  hover(protocol::TextDocumentPositionParams position) = 0;
  virtual protocol::Location
  definition(protocol::TextDocumentPositionParams position) = 0;

  virtual std::variant<
      std::vector<protocol::CompletionItem> /* , CompletionList */> // TODO
  completion(protocol::CompletionParams params) = 0;

  virtual void didOpen(protocol::DidOpenTextDocumentParams params) = 0;
  virtual void didChange(protocol::DidChangeTextDocumentParams params) = 0;
  virtual void didClose(protocol::DidCloseTextDocumentParams params) = 0;
  virtual void didSave(protocol::DidSaveTextDocumentParams params) = 0;

  virtual ~TextDocumentService(){};
};

class CustomMessageService {
public:
  virtual protocol::extensions::Dependencies
  dependencies(protocol::TextDocumentPositionParams position) = 0;
};

/**
 * Language server implementations need to implement the required virtual
 * methods.
 * See
 * https://microsoft.github.io/language-server-protocol/specifications/specification-3-14/
 * and lscpp/protocol for the parameter and result specification.
 */
class lsp_server {
public:
  virtual protocol::InitializeResult
  initialize(protocol::InitializeParams const &params) = 0;

  virtual TextDocumentService &getTextDocumentService() = 0;
  virtual CustomMessageService &getCustomMessageService() {
    throw std::runtime_error("not implemented");
  }

  virtual ~lsp_server(){};
};

} // namespace lscpp
