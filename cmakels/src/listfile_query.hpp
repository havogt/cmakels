#pragma once

#include "listfile_parser.hpp" // TODO move typedefs in separate header
#include <string>

namespace cmake_query {

struct textdocument_position {
  std::size_t line;
  std::size_t character;
};

struct get_function_result_t {
  // TODO extend for incomplete functions

  static constexpr int delimiter_selected = -1;
  static constexpr int function_name_selected = 0;
  // n >= 1: n-th argument selected

  listfile_function_t function;
  int id;

  // Maybe save the token string. There would some redundancy between
  // (id,function) and the token, the only information which we would gain is
  // the current selected delimiter, e.g. spaces or parenthesis.
  // std::string token;
};

// From LSP spec:
// Character offset on a line in a document (zero-based). Assuming that the line
// is represented as a string, the `character` value represents the gap between
// the`character` and `character + 1`.
get_function_result_t get_function(listfile_t const &lf,
                                   textdocument_position pos);

std::string get_selected_token(get_function_result_t const &result);

} // namespace cmake_query
