#pragma once

#include <string>

namespace lscpp {

class stdio_transporter {
public:
  char read_char();
  std::string read_message(std::size_t length);
  void write_message(std::string str);
};

} // namespace lscpp
