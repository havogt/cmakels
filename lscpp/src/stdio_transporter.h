#pragma once

#include "transporter.h"

namespace lscpp {

class stdio_transporter : public transporter {
 public:
  char read_char() override;
  std::string read_message(std::size_t length) override;
  void write_message(std::string str) override;
  ~stdio_transporter();
};

}  // namespace lscpp
