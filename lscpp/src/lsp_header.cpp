#include <cstring>
#include <loguru.hpp>

#include "lsp_header.h"
#include "transporter.h"

namespace lscpp {

namespace {
constexpr char char_lf = char(0x0A);
constexpr char char_cr = char(0x0D);
}  // namespace

// TODO clean this ugly piece
lsp_header parse_header(transporter& t) {
  char content_length[17];
  for (size_t i = 0; i < 16; ++i) {
    content_length[i] = t.read_char();
  }
  content_length[16] = '\0';
  if (std::strcmp(content_length, "Content-Length: ")) {
    std::string msg;
    msg.append(content_length, 16);
    LOG_F(ERROR, "Failed parsing header, got Content-Length: '%s'",
          msg.c_str());
    return {-1};
  }

  const int max_length = 16;
  char size_buffer[max_length];
  int pos = 0;
  bool cr_reached = false;
  bool valid_header = false;
  for (int i = 0; i < max_length - 2; ++i) {
    size_buffer[pos] = t.read_char();
    if (cr_reached) {
      if (size_buffer[pos] == char_lf) {
        valid_header = true;
      }
      break;
    }
    if (size_buffer[pos] == char_cr) cr_reached = true;
    ++pos;
  }
  if (valid_header) {
    size_buffer[pos - 1] = '\0';

    char last_crlf[2];
    last_crlf[0] = t.read_char();
    last_crlf[1] = t.read_char();

    if (last_crlf[0] == char_cr && last_crlf[1] == char_lf) {
      LOG_F(INFO, "Returning Content-Length: %s", size_buffer);
      return {atoi(size_buffer)};
    } else {
      valid_header = false;
    }
  }
  LOG_F(ERROR, "Failed parsing header!");
  return {-1};
}

lsp_header parse_header(transporter&& t) { return parse_header(t); }

}  // namespace lscpp
