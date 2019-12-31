#pragma once

#include <chrono>
#include <future>
#include <sstream>
#include <thread>

#include "lsp_header.h"
#include "lsp_message_handler.h"
#include "lsp_server.h"
#include "stdio_transporter.h"
#include "transporter.h"

namespace lscpp {

inline std::string add_lsp_header(std::string content) {
  std::stringstream reply;
  reply << "Content-Length: ";
  reply << content.size();
#ifdef _WIN32
  reply << "\n\n";
#else
  reply << "\r\n\r\n";
#endif
  reply << content;
  return reply.str();
}

struct launch_config {
  int startup_delay = 0; // in seconds
};

template <class MessageHandler = lsp_message_handler>
void launch(lsp_server &&server, launch_config config = {},
            transporter &&transporter_ = stdio_transporter{},
            MessageHandler &&message_handler = {}) {

  // Allows to attach a debugger,
  // before the language server starts to communicate with the client.
  std::this_thread::sleep_for(std::chrono::seconds(config.startup_delay));

  auto rcv = std::async(std::launch::async, [&]() {
    while (true) {
      auto header = parse_header(transporter_);
      LOG_F(INFO, "content-length: '%d'", header.content_length);
      if (header.content_length <
          0) { // TODO parse header should not use -1 to report an error
        continue;
      }
      auto msg = transporter_.read_message(header.content_length);
      LOG_F(INFO, "msg: '%s'", msg.c_str());

      auto result = message_handler.handle_message(server, msg);
      if (result) {
        LOG_F(INFO, "Sending response: '%s'", (*result).c_str());
        transporter_.write_message(add_lsp_header(*result));
      }
    }
  });

  rcv.wait();
}

} // namespace lscpp
