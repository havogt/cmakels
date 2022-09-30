/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include <any>
#include <string>

#include "lscpp/transporter.h"

#include "lscpp/protocol/CompletionItem.h"
#include "lscpp/protocol/CompletionParams.h"
#include "lscpp/protocol/DidChangeTextDocumentParams.h"
#include "lscpp/protocol/DidCloseTextDocumentParams.h"
#include "lscpp/protocol/DidOpenTextDocumentParams.h"
#include "lscpp/protocol/DidSaveTextDocumentParams.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/TextDocumentPositionParams.h"

namespace lscpp::experimental {

enum class method_kind {
  INITIALIZE,
  INITIALIZED,
  EXIT,
  SHUTDOWN,
  TEXT_DOCUMENT_DID_OPEN,
  TEXT_DOCUMENT_DID_CHANGE,
  TEXT_DOCUMENT_DID_CLOSE,
  TEXT_DOCUMENT_DID_SAVE,
  TEXT_DOCUMENT_HOVER,
  TEXT_DOCUMENT_DEFINITION,
  TEXT_DOCUMENT_COMPLETION,
};

template <method_kind Kind> struct to_param;
template <> struct to_param<method_kind::INITIALIZE> {
  using type = protocol::InitializeParams;
};
template <> struct to_param<method_kind::INITIALIZED> { using type = void; };
template <> struct to_param<method_kind::EXIT> { using type = void; };
template <> struct to_param<method_kind::SHUTDOWN> { using type = void; };
template <> struct to_param<method_kind::TEXT_DOCUMENT_HOVER> {
  using type = protocol::TextDocumentPositionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DEFINITION> {
  using type = protocol::TextDocumentPositionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_COMPLETION> {
  using type = protocol::CompletionParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_OPEN> {
  using type = protocol::DidOpenTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_CHANGE> {
  using type = protocol::DidChangeTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_CLOSE> {
  using type = protocol::DidCloseTextDocumentParams;
};
template <> struct to_param<method_kind::TEXT_DOCUMENT_DID_SAVE> {
  using type = protocol::DidSaveTextDocumentParams;
};

template <method_kind KIND> using to_params_t = typename to_param<KIND>::type;
struct message {
  method_kind method;
  int id;
  std::any params; // variant?
};

std::string response_message(int id);
template <typename Result>
std::string response_message(int id, Result const &result);

void write_lsp_message(transporter &t, std::string const &content);
std::string make_notification_message(std::string const &message);

message parse_request(std::string);
} // namespace lscpp::experimental
