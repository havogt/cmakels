#pragma once

#include "lscpp/protocol/CompletionItem.h"
#include "lscpp/protocol/Hover.h"
#include "lscpp/protocol/InitializeParams.h"
#include "lscpp/protocol/InitializeResult.h"
#include "lscpp/protocol/Location.h"
#include "lscpp/transporter.h"
#include <any>
#include <string>
#include <vector>

namespace lscpp::experimental {
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

std::string response_message(int id);
template <typename Result>
std::string response_message(int id, Result const &result);

void write_lsp_message(transporter &t, std::string const &content);
std::string make_notification_message(std::string const &message);

message parse_request(std::string);
} // namespace lscpp::experimental
