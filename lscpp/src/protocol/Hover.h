#pragma once

#include "Range.h"
#include <optional>
#include <string>

namespace lscpp {
namespace protocol {

/**
 * The result of a hover request.
 */
struct Hover {
  /**
   * The hover's content
   */
  std::string contents; // TODO
  //   contents : MarkedString | MarkedString[] | MarkupContent;

  /**
   * An optional range is a range inside a text document
   * that is used to visualize a hover, e.g. by changing the background color.
   */
  std::optional<Range> range;
};

} // namespace protocol
} // namespace lscpp
