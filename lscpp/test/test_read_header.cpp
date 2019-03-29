#include "../src/lsp_header.h"
#include "../src/transporter.h"

#include <gtest/gtest.h>

namespace lscpp {
class fake_transporter : public transporter {
  std::string msg_;
  std::size_t pos_ = 0;

 public:
  fake_transporter(std::string msg) : msg_{msg} {}

  char read_char() override {
    char c = msg_[pos_];
    pos_++;
    return c;
  }

  std::string read_message(std::size_t len) {
    std::string res = msg_.substr(pos_, len);
    pos_ += len;
    return res;
  }

  void write_message(std::string) {}
};

TEST(parse_header, simple) {
  int expected_content_length = 2;
  fake_transporter t{"Content-Length: 2\r\n\r\n{}"};

  auto result = parse_header(t);

  ASSERT_EQ(expected_content_length, result.content_length);
}
}  // namespace lscpp
