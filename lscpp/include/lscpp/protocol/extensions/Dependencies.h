/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "../Range.h"
#include <optional>
#include <string>
#include <vector>

namespace lscpp {
namespace protocol {
namespace extensions {

struct DependencyEntry {
  /**
   * Identifier of the node
   */
  std::string id;
  // TODO  string | number;

  /**
   * List of children, identified by `id`.
   */
  std::vector<std::string> children;
  // TODO string[] | number[];

  /**
   * The node's content.
   */
  std::string content;
  // TODO MarkupContent
};

struct Dependencies {
  /**
   * The dependency graph as a list of `DependencyEntry`ies
   */
  std::vector<DependencyEntry> dependencies;

  /**
   * An optional range is a range inside a text document
   * that is used to visualize a dependency request, e.g. by changing the
   * background color.
   */
  std::optional<Range> range;
};

} // namespace extensions
} // namespace protocol
} // namespace lscpp
