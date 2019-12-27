#pragma once

#include <cstdio>
#include <memory>
#include <optional>
#include <string>

namespace lscpp {

struct comm_logger {
  std::size_t msg_id_ = 0;
  bool file_open_ = false;
  std::FILE *cur_file_;

  ~comm_logger();
  void open(std::string suffix);
  void open_in();
  void open_out();
  void close();
  void write(const void *buf, std::size_t size);
};

class stdio_transporter {
private:
  std::optional<comm_logger> comm_logger_;

  std::size_t size_ = 1024;
  std::unique_ptr<char[]> data_{new char[size_]};

  void reserve(std::size_t size);

public:
  stdio_transporter() = default;
  stdio_transporter(bool log_communication);

  char read_char();
  std::string read_message(std::size_t length);
  void write_message(std::string str);
};

} // namespace lscpp
