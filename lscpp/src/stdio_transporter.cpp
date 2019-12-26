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

void stdio_transporter::reserve(std::size_t size) {
  if (size > size_) {
    size_ *= 2;
    data_.reset(new char[size_]);
  }
}

char stdio_transporter::read_char() {
  char c;
  ::read(stdin_fileno, &c, 1);
  return c;
}

std::string stdio_transporter::read_message(std::size_t length) {
  reserve(length);
  ::read(stdin_fileno, data_.get(), length);
  std::string res;
  res.append(data_.get(), length);
  return res;
}

void stdio_transporter::write_message(std::string str) {
  const char *cstr = str.c_str();
  std::size_t sizeof_reply = str.size();
  ::write(stdout_fileno, cstr, sizeof_reply);
}

#undef stdin_fileno
#undef stdout_fileno

} // namespace lscpp
