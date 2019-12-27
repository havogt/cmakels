#include "stdio_transporter.h"
#include <optional>

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

stdio_transporter::stdio_transporter(bool log_communication)
    : comm_logger_{log_communication ? std::optional<comm_logger>{comm_logger{}}
                                     : std::nullopt} {}

void stdio_transporter::reserve(std::size_t size) {
  bool reallocate = false;
  while (size > size_) {
    reallocate = true;
    size_ *= 2;
  }
  data_.reset(new char[size_]);
}

char stdio_transporter::read_char() {
  char c;
  ::read(stdin_fileno, &c, 1);
  if (comm_logger_) {
    if (!comm_logger_->file_open_)
      comm_logger_->open_in();
    comm_logger_->write(&c, 1);
  }
  return c;
}

std::string stdio_transporter::read_message(std::size_t length) {
  reserve(length);
  ::read(stdin_fileno, data_.get(), length);
  if (comm_logger_) {
    if (!comm_logger_->file_open_)
      comm_logger_->open_in();
    comm_logger_->write(data_.get(), length);
    comm_logger_->close();
  }
  std::string res;
  res.append(data_.get(), length);
  return res;
}

void stdio_transporter::write_message(std::string str) {
  const char *cstr = str.c_str();
  std::size_t sizeof_reply = str.size();
  ::write(stdout_fileno, cstr, sizeof_reply);
  if (comm_logger_) {
    if (comm_logger_->file_open_)
      comm_logger_->close();
    comm_logger_->open_out();
    comm_logger_->write(cstr, sizeof_reply);
    comm_logger_->close();
  }
}

#undef stdin_fileno
#undef stdout_fileno

} // namespace lscpp
