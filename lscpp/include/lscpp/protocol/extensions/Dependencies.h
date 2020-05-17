/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "../Range.h"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace lscpp {
namespace protocol {
namespace extensions {

using DependencyNodeId = std::variant<int, std::string>;

enum class DependencyEdgeKind { Solid = 1, Dashed = 2, Dotted = 3, Bold = 4 };

/**
 * An edge in the dependency graph
 */
struct DependencyEdge {
  /**
   * Parent
   */
  DependencyNodeId from;

  /**
   * Child
   */
  DependencyNodeId to;

  /**
   * Edge kind
   */
  DependencyEdgeKind kind;
};

/**
 * A node in the dependency graph
 */
struct DependencyNode {
  /**
   * Identifier of the node
   */
  DependencyNodeId id;

  /**
   * The node's content.
   */
  std::string content; // TODO
                       // MarkupContent content;
};

/**
 * The result of a dependency request.
 */
struct Dependencies {
  /**
   * The dependency graph as a list of `DependencyNode`s ...
   */
  std::vector<DependencyNode> nodes;

  /**
   * ... and a list of edges between nodes.
   */
  std::vector<DependencyEdge> edges;

  /**
   * An optional root, identifies the root node, if any.
   */
  std::optional<DependencyNodeId> root;

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
