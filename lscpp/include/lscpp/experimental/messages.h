#pragma once
#include "lscpp/protocol/Hover.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include <any>
#include <string>

namespace lscpp {

struct request {
  std::string type;
  int id;
  std::any data;
};

std::string
initialize_response(int id, lscpp::protocol::InitializeResult const &result);
std::string hover_response(int id, lscpp::protocol::Hover const &result);
std::string shutdown_response(int id);
std::string notification_message(std::string const &message);

request parse_request(std::string);

// protocol::InitializeParams parse_initialize_params()
} // namespace lscpp
