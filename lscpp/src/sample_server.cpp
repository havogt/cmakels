#include <future>
#include <loguru.hpp>
#include <queue>
#include <sstream>
#include <string>

#include "lsp_header.h"
#include "messages.h"
#include "stdio_transporter.h"

namespace lscpp {

std::string make_lsp_message(std::string content) {
  std::stringstream reply;
  reply << "Content-Length: ";
  reply << content.size();
  reply << "\r\n\r\n";
  reply << content;
  return reply.str();
}

class lsp_server {
public:
  stdio_transporter transporter_;

  void start() {
    auto rcv = std::async(std::launch::async, [this]() {
      while (true) {
        auto header = parse_header(transporter_);
        LOG_F(INFO, "content-length: '%d'", header.content_length);
        auto msg = transporter_.read_message(header.content_length);
        LOG_F(INFO, "msg: '%s'", msg.c_str());
        queue_.emplace(msg);
      }
    });

    auto process = std::async(std::launch::async, [this]() {
      while (true) {
        if (queue_.size() > 0) {
          std::string message = queue_.back();
          queue_.pop();

          nlohmann::json m_as_json = nlohmann::json::parse(message);
          LOG_F(INFO, "'%s'", m_as_json.dump().c_str());
          if (m_as_json.contains("id")) {
            message::RequestMessage my_message =
                m_as_json.get<message::RequestMessage>();
            nlohmann::json and_back = my_message;
            LOG_F(INFO, "'%s'", and_back.dump().c_str());

            // assume that it was "initialize"
            {
              std::string reply_content =
                  "{\"jsonrpc\":\"2.0\",\"id\":0,\"result\":{"
                  "\"capabilities\":{"
                  "}}}";

              transporter_.write_message(make_lsp_message(reply_content));
            }
            {
              std::string reply_content =
                  "{\"jsonrpc\":\"2.0\",\"method\":\"window/showMessage\""
                  ",\"params\":{\"type\":1,\"message\":\"Oh no2!\"}}";
              LOG_F(INFO, "'%s'", make_lsp_message(reply_content).c_str());
              transporter_.write_message(make_lsp_message(reply_content));
            }
          } else {
            message::NotificationMessage my_message =
                m_as_json.get<message::NotificationMessage>();
            nlohmann::json and_back = my_message;

            LOG_F(INFO, "'%s'", and_back.dump().c_str());
          }
        }
      }
    });

    rcv.wait();
    process.wait();
  }

private:
  std::queue<std::string> queue_; // TODO make it threadsafe
};

} // namespace lscpp

int main(int argc, char *argv[]) {
  loguru::g_stderr_verbosity = loguru::Verbosity_OFF;
  loguru::g_colorlogtostderr = false;
  loguru::init(argc, argv);
  loguru::add_file("sample_server.log", loguru::Truncate,
                   loguru::Verbosity_MAX);

  lscpp::lsp_server serv;
  serv.start();
}
