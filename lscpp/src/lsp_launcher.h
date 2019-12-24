#pragma once

#include <future>
#include <sstream>

#include "lsp_header.h"
#include "lsp_message_handler.h"
#include "lsp_server.h"
#include "stdio_transporter.h"

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

template <class MessageHandler = lsp_message_handler> class lsp_launcher {
public:
  MessageHandler msg_handler_;

  template <class Server, class Transporter = stdio_transporter>
  void start(Server &&server, Transporter &&transporter = {}) {
    auto rcv = std::async(std::launch::async, [&]() {
      while (true) {
        auto header = parse_header(transporter);
        LOG_F(INFO, "content-length: '%d'", header.content_length);
        auto msg = transporter.read_message(header.content_length);
        LOG_F(INFO, "msg: '%s'", msg.c_str());

        auto result = msg_handler_.handle_message(server, msg);
        if (result) {
          LOG_F(INFO, "Sending response: '%s'", (*result).c_str());
          transporter.write_message(add_lsp_header(*result));
        }
      }
    });

    rcv.wait();
  }
};

} // namespace lscpp
