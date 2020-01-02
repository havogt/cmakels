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

  LOG_F(ERROR, "Failed parsing header!");
  return {-1};
} // namespace lscpp

lsp_header parse_header(transporter &&t) { return parse_header(t); }

} // namespace lscpp
