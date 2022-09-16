#pragma once

#include <any>
#include <string>

#include "lscpp/transporter.h"

namespace lscpp::experimental {
struct message {
  std::string method;
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
