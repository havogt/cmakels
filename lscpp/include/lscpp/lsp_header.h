/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <optional>
#include <string>

#include <lscpp/transporter.h>

namespace lscpp {
struct lsp_header {
  int content_length;
  std::optional<std::string> content_type;
};

lsp_header parse_header(transporter &t);
lsp_header parse_header(transporter &&t);
} // namespace lscpp
