/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

namespace lscpp {
namespace protocol {

struct Position {
  /**
   * Line position in a document (zero-based).
   */
  int line;

  /**
   * Character offset on a line in a document (zero-based). Assuming that the
   * line is represented as a string, the `character` value represents the gap
   * between the `character` and `character + 1`.
   *
   * If the character value is greater than the line length it defaults back to
   * the line length.
   */
  int character;
};

} // namespace protocol
} // namespace lscpp
