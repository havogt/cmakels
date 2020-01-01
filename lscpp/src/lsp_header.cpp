#include <cstring>
#include <loguru.hpp>

#include "lsp_header.h"
#include "transporter.h"

namespace lscpp {

namespace {
constexpr char char_lf = char(0x0A);
constexpr char char_cr = char(0x0D);
} // namespace

lsp_header parse_header(transporter &t) {
  int content_length;
  if (auto line = t.read_line();
      line.substr(0, 16).compare("Content-Length: ") == 0) {
    content_length = std::stoi(line.substr(16, std::string::npos));
  } else {
    LOG_F(ERROR,
          "Failed parsing header, got '%s' instead of 'Content-Length: '",
          line.substr(0, 16).c_str());
    return {-1}; // TODO return a proper error on failure.
  }

  if (auto line = t.read_line(); line.size() == 0) // a new line
    return {content_length};                       // valid header

  // // TODO the transporter has to deal with the line endings
  // // Probably replace read_char by read_line
  // #ifdef _WIN32
  //   if (size_buffer[pos] == char_lf) {
  //     valid_header = true;
  //     ++pos;
  //     break;
  //   }
  // #else
  //   if (cr_reached) {
  //     if (size_buffer[pos] == char_lf) {
  //       valid_header = true;
  //     }
  //     break;
  //   }
  //   if (size_buffer[pos] == char_cr)
  //     cr_reached = true;
  // #endif
  //   ++pos;
  //   LOG_F(INFO, "Size buffer: %c", size_buffer[pos - 1]);
  // }
  // if (valid_header) {
  //   LOG_F(INFO, "valid_header");
  //   size_buffer[pos - 1] = '\0';

  //   char last_crlf[2];
  // #ifdef _WIN32
  //   last_crlf[0] = t.read_char();
  //   if (last_crlf[0] == char_lf) {
  //     LOG_F(INFO, "Returning Content-Length: %s", size_buffer);
  //     return {atoi(size_buffer)};
  //   } else {
  //     valid_header = false;
  //   }
  // #else
  //   last_crlf[0] = t.read_char();
  //   last_crlf[1] = t.read_char();

  //   if (last_crlf[0] == char_cr && last_crlf[1] == char_lf) {
  //     LOG_F(INFO, "Returning Content-Length: %s", size_buffer);
  //     return {atoi(size_buffer)};
  //   } else {
  //     valid_header = false;
  //   }
  // #endif
  // }
  LOG_F(ERROR, "Failed parsing header!");
  return {-1};
} // namespace lscpp

lsp_header parse_header(transporter &&t) { return parse_header(t); }

} // namespace lscpp
