#pragma once

#include <cstdio>
#include <memory>
#include <optional>
#include <string>

namespace lscpp {

// The logger is currently implemented using the same technique as the
// stdio_transporter uses for communication (i.e. file descriptors). This is a
// very conservative choice as I am not very familiar with how different i/o
// routines behave on different platforms concerning line endings.
struct comm_logger {
public:
  ~comm_logger();
  void open_in();
  void open_out();
  void close();
  void write(const void *buf, std::size_t size);

private:
  std::size_t msg_id_ = 0;
  bool file_open_ = false;
  std::FILE *cur_file_;
  void open(std::string suffix);
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
  std::string read_line();
  std::string read_message(std::size_t length);
  void write_message(std::string str);
};

} // namespace lscpp
