#include "stdio_transporter.h"

#include <unistd.h>
namespace lscpp {

char stdio_transporter::read_char() {
  char c;
  ::read(STDIN_FILENO, &c, 1);
  return c;
}

std::string stdio_transporter::read_message(std::size_t length) {
  char* str = new char[length];
  ::read(STDIN_FILENO, str, length);
  std::string res;
  res.append(str, length);
  delete[] str;  // TODO pre-allocated buffer
  return res;
}

void stdio_transporter::write_message(std::string str) {
  const char* cstr = str.c_str();
  std::size_t sizeof_reply = str.size();
  ::write(STDOUT_FILENO, cstr, sizeof_reply);
}

stdio_transporter::~stdio_transporter() {}

}  // namespace lscpp
