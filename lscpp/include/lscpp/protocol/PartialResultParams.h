/*
 * Copyright 2019-2022 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once
#include <optional>
#include <string>
#include <variant>

namespace lscpp {
namespace protocol {

using ProgressToken = std::variant<int, std::string>;

struct PartialResultParams {
  /**
   * An optional token that a server can use to report partial results (e.g.
   * streaming) to the client.
   */
  std::optional<ProgressToken> partialResultToken;
};

} // namespace protocol
} // namespace lscpp
