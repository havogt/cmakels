#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <future>
#include <queue>
#include <sstream>

#include "messages.h"

namespace {
constexpr char char_lf = char(0x0A);
constexpr char char_cr = char(0x0D);

// TODO this doesn't work if "Content-type" is present
int read_header(int fd) {
  char content_length[16];
  ::read(fd, content_length, 16);

  const int max_length = 16;
  char size_buffer[max_length];
  int pos = 0;
  bool cr_reached = false;
  bool valid_header = false;
  for (int i = 0; i < max_length - 2; ++i) {
    ::read(fd, &size_buffer[pos], 1);
    if (cr_reached) {
      if (size_buffer[pos] == char_lf) {
        valid_header = true;
      }
      break;
    }
    if (size_buffer[pos] == char_cr) cr_reached = true;
    ++pos;
  }
  if (valid_header) {
    size_buffer[pos - 1] = '\0';

    char last_crlf[2];
    ::read(fd, last_crlf, 2);

    if (last_crlf[0] == char_cr && last_crlf[1] == char_lf) {
      return atoi(size_buffer);
    } else {
      valid_header = false;
    }
  }
  return -1;
}
}  // namespace

class lsp_server {
 public:
  void start() {
    auto rcv = std::async(std::launch::async, [this]() {
      while (true) {
        int content_length = read_header(STDIN_FILENO);
        char* buffer = new char[content_length];

        ::read(STDIN_FILENO, buffer, content_length);
        std::string tmp;
        tmp.append(buffer, content_length);
        queue_.emplace(tmp);
        delete[] buffer;
      }
    });

    std::ofstream of(
        "/home/vogtha/workspace-cpp-lsp/sample_lsp_server2/log/log.json");
    auto process = std::async(std::launch::async, [this, &of]() {
      while (true) {
        if (queue_.size() > 0) {
          std::string message = queue_.back();
          queue_.pop();

          nlohmann::json m_as_json = nlohmann::json::parse(message);
          of << m_as_json.dump() << std::endl;
          if (m_as_json.contains("id")) {
            message::RequestMessage my_message =
                m_as_json.get<message::RequestMessage>();
            nlohmann::json and_back = my_message;
            of << and_back.dump() << std::endl;

            // assume that it was "initialize"
            {
              std::string reply_content =
                  "{\"jsonrpc\":\"2.0\",\"id\":0,\"result\":{"
                  "\"capabilities\":{"
                  "}}}";
              std::stringstream reply;
              reply << "Content-Length: ";
              reply << reply_content.size();
              reply << "\r\n\r\n";
              reply << reply_content;
              auto reply_as_string = reply.str();
              const char* reply_cstr = reply_as_string.c_str();
              const int sizeof_reply = reply_as_string.size();
              size_t written_bytes =
                  ::write(STDOUT_FILENO, reply_cstr, sizeof_reply);
            }
            {
              std::string reply_content =
                  "{\"jsonrpc\":\"2.0\",\"method\":\"window/showMessage\""
                  ",\"params\":{\"type\":1,\"message\":\"Oh no!\"}}";
              std::stringstream reply;
              reply << "Content-Length: ";
              reply << reply_content.size();
              reply << "\r\n\r\n";
              reply << reply_content;
              auto reply_as_string = reply.str();
              const char* reply_cstr = reply_as_string.c_str();
              const int sizeof_reply = reply_as_string.size();
              size_t written_bytes =
                  ::write(STDOUT_FILENO, reply_cstr, sizeof_reply);
            }
          } else {
            message::NotificationMessage my_message =
                m_as_json.get<message::NotificationMessage>();
            nlohmann::json and_back = my_message;
            of << and_back.dump() << std::endl;
          }
        }
      }
    });

    rcv.wait();
    process.wait();
    of.close();
  }

 private:
  std::queue<std::string> queue_;  // TODO make it threadsafe
};

int main() {
  lsp_server serv;
  serv.start();
}
