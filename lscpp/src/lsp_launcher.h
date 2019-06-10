#pragma once

#include <future>
#include <sstream>

#include "lsp_header.h"
#include "lsp_message_handler.h"
#include "lsp_server.h"
#include "stdio_transporter.h"
#include "transporter.h"

namespace lscpp {

std::string make_lsp_message(std::string content) {
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
  std::unique_ptr<lsp_server> server_;
  MessageHandler msg_handler_;
  std::unique_ptr<transporter> transporter_;

  lsp_launcher(std::unique_ptr<lsp_server> server)
      : server_{std::move(server)}, msg_handler_{*(server_.get())},
        transporter_{new stdio_transporter{}} {}

  void start() {
    auto rcv = std::async(std::launch::async, [this]() {
      while (true) {
        auto header = parse_header(*transporter_);
        LOG_F(INFO, "content-length: '%d'", header.content_length);
        auto msg = transporter_->read_message(header.content_length);
        LOG_F(INFO, "msg: '%s'", msg.c_str());

        std::string result = msg_handler_.handle_request(msg);
        LOG_F(INFO, "Sending response: '%s'", result.c_str());
        transporter_->write_message(make_lsp_message(result));
      }
    });

    rcv.wait();
  }
};

} // namespace lscpp
