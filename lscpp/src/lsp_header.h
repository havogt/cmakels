#pragma once
#include <optional>
#include <string>
#include "transporter.h"

namespace lscpp {
struct lsp_header {
  int content_length;
  std::optional<std::string> content_type;
};

lsp_header parse_header(transporter& t);
}  // namespace lscpp
