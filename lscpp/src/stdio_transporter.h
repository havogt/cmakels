#pragma once

#include <memory>
#include <string>

namespace lscpp {

class stdio_transporter {
private:
  std::size_t size_ = 1024;
  std::unique_ptr<char[]> data_{new char[size_]};

  void reserve(std::size_t size);

public:
  char read_char();
  std::string read_message(std::size_t length);
  void write_message(std::string str);
};

} // namespace lscpp
