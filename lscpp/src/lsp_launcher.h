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

inline void write_lsp_message(transporter &t, std::string const &content) {
  std::stringstream content_length;
  content_length << "Content-Length: ";
  content_length << content.size();
  t.write_line(content_length.str());
  t.write_line("");

  t.write_message(content);
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
        write_lsp_message(transporter_, *result);
      }
    }
  });

  rcv.wait();
}

} // namespace lscpp
