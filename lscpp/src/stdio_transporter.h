#pragma once

#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <unistd.h>

#include <iomanip>
#include <sstream>

namespace lscpp {

struct comm_logger {
  std::size_t msg_id_ = 0;
  bool file_open_ = false;
  std::FILE *cur_file_;

  ~comm_logger() {
    if (file_open_) {
      fclose(cur_file_);
    }
  }

  std::string filename(std::string suffix) {
    std::stringstream ss;
    ss << ".lscpp/" << std::setw(3) << std::setfill('0') << msg_id_ << suffix;
    return ss.str();
  }

  void open(std::string suffix) {
    auto name = filename(suffix);
    cur_file_ = std::fopen(name.c_str(), "w");
    file_open_ = true;
    msg_id_++;
  }

  void open_in() { open(".in"); }
  void open_out() { open(".out"); }

  void close() {
    file_open_ = false;
    std::fclose(cur_file_);
  }

  void write(const void *buf, std::size_t size) {
    ::write(fileno(cur_file_), buf, size);
  }
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
