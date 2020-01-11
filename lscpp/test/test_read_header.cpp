/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "../src/lsp_header.h"
#include <lscpp/transporter.h>

#include <gtest/gtest.h>

namespace lscpp {
class fake_transporter {
  std::string msg_;
  std::size_t pos_ = 0;

public:
  fake_transporter(std::string msg) : msg_{msg} {}

  char read_char() {
    char c = msg_[pos_];
    pos_++;
    return c;
  }

  std::string read_line() {
    std::string tmp;
    while (true) {
      char c = read_char();
      tmp.append(1, c);
      if (tmp.size() >= 2 && tmp.substr(tmp.size() - 2).compare("\r\n") == 0)
        break;
    }
    return tmp.substr(0, tmp.size() - 2);
  }

  std::string read_message(std::size_t len) {
    std::string res = msg_.substr(pos_, len);
    pos_ += len;
    return res;
  }

  void write_message(std::string, bool) {}
};

TEST(parse_header, simple) {
  int expected_content_length = 2;
  auto result = parse_header(fake_transporter{"Content-Length: 2\r\n\r\n{}"});

  ASSERT_EQ(expected_content_length, result.content_length);
}
} // namespace lscpp
