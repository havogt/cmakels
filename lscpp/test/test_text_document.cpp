#include "../src/helper/text_document.h"
#include "/home/vogtha/lsp/experiments/cmakels/lscpp/src/protocol/Range.h"

#include <gtest/gtest.h>

namespace lscpp {

TEST(text_document, simple) {
  text_document doc;

  auto result = get_range(doc, protocol::Range{{0, 1}, {1, 2}});

  ASSERT_EQ(" Top Level CMakeLists.txt File\ncm", result);
}

} // namespace lscpp
