#pragma once

#include "../protocol/Range.h"
#include <string>
#include <vector>

namespace lscpp {
class text_document {
private:
  std::vector<std::string> lines = {"# Top Level CMakeLists.txt File",
                                    "cmake_minimum_required( VERSION 2.8 )",
                                    "# set the installation prefix",
                                    "# Note: setting this *before* the project "
                                    "imposes a default that the user is"};

public:
  std::string get_line(std::size_t i) const { return lines[i]; }
};

std::string get_range(text_document const &doc, protocol::Range const &range) {
  std::string result =
      doc.get_line(range.start.line)
          .substr(range.start.character, range.end.line == range.start.line
                                             ? range.end.character
                                             : std::string::npos);
  for (size_t i = range.start.line + 1; i < range.end.line - 1; ++i) {
    result += "\n" + doc.get_line(i);
  }
  if (range.end.line > range.start.line) {
    result +=
        "\n" + doc.get_line(range.end.line).substr(0, range.end.character);
  }
  return result;
}

} // namespace lscpp
