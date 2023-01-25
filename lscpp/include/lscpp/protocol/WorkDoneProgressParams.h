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

struct WorkDoneProgressParams {
  /**
   * An optional token that a server can use to report work done progress.
   */
  std::optional<ProgressToken> workDoneToken;
};

} // namespace protocol
} // namespace lscpp
