/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "listfile_query.hpp"
#include "listfile_parser.hpp"

#include <stdexcept>

namespace cmake_query {

namespace {
bool is_in_function(listfile_function_t const &f, textdocument_position pos) {
  if (pos.line > get_line(f) ||
      (pos.line == get_line(f) && pos.character >= get_column(f))) {
    if (pos.line < get_line_end(f) ||
        (pos.line == get_line_end(f) && pos.character < get_column_end(f)))
      return true;
  }
  return false;
}
} // namespace

get_function_result_t get_function(listfile_t const &lf,
                                   textdocument_position pos) {
  for (auto const &f : get_functions(lf)) {
    if (is_in_function(f, pos)) {
      if (pos.line == get_line(f) &&
          pos.character < get_column_functionname_end(f)) {
        return {f, get_function_result_t::function_name_selected};
      }
      int id = 0;
      for (auto const &arg : get_arguments(f)) {
        id++;

        if (pos.line == get_line(arg) && pos.character >= get_column(arg) &&
            pos.character < get_column_end(arg)) {
          return {f, id};
        }
      }
      return {f, get_function_result_t::delimiter_selected};
    }
  }
  throw std::runtime_error("TODO replace me by a useful exception");
}

std::string get_selected_token(get_function_result_t const &result) {
  if (result.id == get_function_result_t::function_name_selected) {
    return get_name(result.function);
  } else if (result.id == get_function_result_t::delimiter_selected) {
    return "";
  } else {
    return get_name(get_arguments(result.function)[result.id - 1]);
  }
}
} // namespace cmake_query
