#pragma once
#include "lscpp/protocol/CompletionItem.h"
#include "lscpp/protocol/Hover.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/Location.h"
#include <any>
#include <string>
#include <vector>

namespace lscpp {
struct notification_message {
  std::any params;
};
struct request_message {
  int id;
  std::any params;
};

struct message {
  std::string method;
  std::variant<notification_message, request_message> data;
};

std::string initialize_response(int id,
                                protocol::InitializeResult const &result);
std::string hover_response(int id, protocol::Hover const &result);
std::string definition_response(int id, protocol::Location const &result);
std::string completion_response(
    int id, std::variant<std::vector<protocol::CompletionItem>> const &result);
std::string shutdown_response(int id);
std::string make_notification_message(std::string const &message);

message parse_request(std::string);
} // namespace lscpp
