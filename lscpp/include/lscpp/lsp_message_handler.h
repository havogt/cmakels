#pragma once

#include "lsp_server.h"

namespace lscpp {

class lsp_message_handler {
  bool initialized_ = false;
  bool ready_ = false;    // after received initialize
  bool shutdown_ = false; // received shutdown request, next is an exit

public:
  std::optional<std::string> handle_message(lsp_server &server,
                                            std::string const &request);
};

} // namespace lscpp
