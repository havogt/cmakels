#include "stdio_transporter.h"

#ifdef _WIN32
#include <io.h>
#define stdin_fileno _fileno(stdin)
#define stdout_fileno _fileno(stdout)
#else
#include <unistd.h>
#define stdin_fileno STDIN_FILENO
#define stdout_fileno STDOUT_FILENO
#endif

namespace lscpp {

char stdio_transporter::read_char() {
  char c;
  ::read(stdin_fileno, &c, 1);
  return c;
}

std::string stdio_transporter::read_message(std::size_t length) {
  char* str = new char[length];
  ::read(stdin_fileno, str, length);
  std::string res;
  res.append(str, length);
  delete[] str;  // TODO pre-allocated buffer
  return res;
}

void stdio_transporter::write_message(std::string str) {
  const char* cstr = str.c_str();
  std::size_t sizeof_reply = str.size();
  ::write(stdout_fileno, cstr, sizeof_reply);
}

stdio_transporter::~stdio_transporter() {}

}  // namespace lscpp
