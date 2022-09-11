#pragma once
#include "lscpp/protocol/Hover.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include <any>
#include <string>

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

std::string
initialize_response(int id, lscpp::protocol::InitializeResult const &result);
std::string hover_response(int id, lscpp::protocol::Hover const &result);
std::string shutdown_response(int id);
std::string make_notification_message(std::string const &message);

message parse_request(std::string);

// protocol::InitializeParams parse_initialize_params()
} // namespace lscpp
