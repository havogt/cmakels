#include <gtest/gtest.h>
#include <listfile_parser.hpp>
#include <stdexcept>
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

namespace {
bool is_in_function(listfile_function_t const &f, textdocument_position pos) {
  return pos.line >= get_line(f) && pos.character >= get_column(f) &&
         pos.line <= get_line_end(f) && pos.character < get_column_end(f);
}
} // namespace

// From LSP spec:
// Character offset on a line in a document (zero-based). Assuming that the line
// is represented as a string, the `character` value represents the gap between
// the`character` and `character + 1`.
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

namespace {

TEST(listfile_query, get_function_name_from_first_char) {
  auto lf = parse_listfile(R"?(add_subdirectory())?");

  auto result = get_function(*lf, {0, 0});

  ASSERT_EQ(get_function_result_t::function_name_selected, result.id);
  ASSERT_EQ("add_subdirectory", get_selected_token(result));
}

TEST(listfile_query, get_function_name_from_last_char) {
  auto lf = parse_listfile(R"?(add_subdirectory())?");

  auto result = get_function(*lf, {0, 15});

  ASSERT_EQ(get_function_result_t::function_name_selected, result.id);
  ASSERT_EQ("add_subdirectory", get_selected_token(result));
}

TEST(listfile_query, get_function_name_beyond_last_char) {
  auto lf = parse_listfile(R"?(add_subdirectory())?");

  auto result = get_function(*lf, {0, 16});
  ASSERT_EQ("add_subdirectory", get_name(result.function));
  ASSERT_EQ(get_function_result_t::delimiter_selected, result.id);
}

TEST(listfile_query, get_function_out_of_range) {
  auto lf = parse_listfile(R"?(add_subdirectory() )?");

  ASSERT_ANY_THROW(get_function(*lf, {0, 18}));
}

TEST(listfile_query, get_function_arg_first_char) {
  auto lf = parse_listfile(R"?(add_subdirectory(my_subdir))?");

  auto result = get_function(*lf, {0, 17});

  int expected_arg_id = 1;
  ASSERT_EQ(expected_arg_id, result.id);
  ASSERT_EQ("my_subdir", get_selected_token(result));
}

TEST(listfile_query, get_function_arg_last_char) {
  auto lf = parse_listfile(R"?(add_subdirectory(my_subdir))?");

  auto result = get_function(*lf, {0, 25});

  ASSERT_EQ("my_subdir", get_selected_token(result));
}

TEST(listfile_query, get_function_arg_beyond_last_char) {
  auto lf = parse_listfile(R"?(add_subdirectory(my_subdir))?");

  auto result = get_function(*lf, {0, 26});

  ASSERT_EQ(get_function_result_t::delimiter_selected, result.id);
}

TEST(listfile_query, get_function_arg_between_args) {
  std::string input = R"?(my_command(arg0 arg1))?";

  auto lf = parse_listfile(input);

  auto result = get_function(*lf, {0, 15});
  ASSERT_EQ("my_command", get_name(result.function));
  ASSERT_EQ(get_function_result_t::delimiter_selected, result.id);
}

// TODO maybe test for "a𐐀b" (where 𐐀 is represented by two code units in
// UTF-16: chars are a=0, 𐐀=1, b=3)

// TODO test for multiline quoted args (once the parse supports it)

} // namespace
} // namespace cmake_query
