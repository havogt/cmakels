#pragma once

#include <string>

namespace lscpp {

class transporter {
 public:
  virtual char read_char() = 0;
  virtual std::string read_message(std::size_t length) = 0;
  virtual void write_message(std::string str) = 0;
  virtual ~transporter(){};
};

}  // namespace lscpp
