#include "lscpp/stdio_transporter.h"
#include <iomanip>
#include <optional>
#include <sstream>

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

namespace {
std::string filename(int id, std::string suffix) {
  std::stringstream ss;
  ss << ".lscpp/" << std::setw(3) << std::setfill('0') << id << suffix;
  return ss.str();
}
} // namespace

comm_logger::~comm_logger() {
  if (file_open_) {
    fclose(cur_file_);
  }
}

void comm_logger::open(std::string suffix) {
  auto name = filename(msg_id_, suffix);
  cur_file_ = std::fopen(name.c_str(), "w");
  file_open_ = true;
  msg_id_++;
}

void comm_logger::open_in() {
  if (!file_open_)
    open(".in");
}
void comm_logger::open_out() {
  if (!file_open_)
    open(".out");
}

void comm_logger::close() {
  file_open_ = false;
  std::fclose(cur_file_);
}

void comm_logger::write(const void *buf, std::size_t size) {
  ::write(fileno(cur_file_), buf, size); // consider writting not via fd
}

namespace {
void log_in_char(std::optional<comm_logger> &logger, char const &c) {
  if (logger) {
    logger->open_in();
    logger->write(&c, 1);
    // leave file open, will be closed after a call to log_in_message()
  }
}

void log_in_message(std::optional<comm_logger> &logger, const void *buf,
                    std::size_t size) {
  if (logger) {
    logger->open_in();
    logger->write(buf, size);
    logger->close();
  }
}

void log_out_message(std::optional<comm_logger> &logger, const void *buf,
                     std::size_t size, bool close_file) {
  if (logger) {
    logger->open_out();
    logger->write(buf, size);
    if (close_file)
      logger->close();
  }
}
} // namespace

stdio_transporter::stdio_transporter(bool log_communication)
    : comm_logger_{log_communication ? std::optional<comm_logger>{comm_logger{}}
                                     : std::nullopt} {}

void stdio_transporter::reserve(std::size_t size) {
  bool reallocate = false;
  while (size > size_) {
    reallocate = true;
    size_ *= 2;
  }
  if (reallocate)
    data_.reset(new char[size_]);
}

char stdio_transporter::read_char() {
  char c;
  ::read(stdin_fileno, &c, 1);

  log_in_char(comm_logger_, c);

  return c;
}

std::string stdio_transporter::read_message(std::size_t length) {
  reserve(length);
  ::read(stdin_fileno, data_.get(), length);

  log_in_message(comm_logger_, data_.get(), length);

  std::string res;
  res.append(data_.get(), length);
  return res;
}

void stdio_transporter::write_message_impl(std::string str, bool close_file) {
  const char *cstr = str.c_str();
  std::size_t sizeof_reply = str.size();
  ::write(stdout_fileno, cstr, sizeof_reply);
  log_out_message(comm_logger_, cstr, sizeof_reply, close_file);
}

void stdio_transporter::write_message(std::string const &str, bool newline) {
  if (newline) {
#ifdef _WIN32
    write_message_impl(str + "\n");
#else
    write_message_impl(str + "\r\n");
#endif
  } else
    write_message_impl(str, true);
}

namespace {
constexpr char char_lf = char(0x0A);
constexpr char char_cr = char(0x0D);
} // namespace

std::string stdio_transporter::read_line() {
  char c = read_char();
  std::size_t pos = 0;
  std::string line;
#ifdef _WIN32
  while (c != char_lf) {
    reserve(pos);
    data_.get()[pos] = c;
    pos++;
    c = read_char();
  }
  line.append(data_.get(), pos);
#else
  while (c != char_cr) {
    reserve(pos);
    data_.get()[pos] = c;
    pos++;
    c = read_char();
  }
  c = read_char();
  if (c == char_lf) {
    line.append(data_.get(), pos);
  } else {
    // TODO error
  }
#endif
  return line;
}

#undef stdin_fileno
#undef stdout_fileno

} // namespace lscpp
