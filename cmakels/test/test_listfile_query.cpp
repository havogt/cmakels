#include <gtest/gtest.h>
#include <listfile_parser.hpp>
#include <listfile_query.hpp>

namespace cmake_query {
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

TEST(listfile_query, multiline_with_args_and_spaces) {
  auto lf = parse_listfile(R"?(target_link_libraries( a_target
PRIVATE  some_lib))?");

  ASSERT_EQ("target_link_libraries",
            get_selected_token(get_function(*lf, {0, 2})));
  ASSERT_EQ("a_target", get_selected_token(get_function(*lf, {0, 25})));
  ASSERT_EQ("PRIVATE", get_selected_token(get_function(*lf, {1, 0})));
  ASSERT_EQ("some_lib", get_selected_token(get_function(*lf, {1, 10})));
}

// TODO maybe test for "a𐐀b" (where 𐐀 is represented by two code units in
// UTF-16: chars are a=0, 𐐀=1, b=3)

// TODO test for multiline quoted args (once the parse supports it)

} // namespace
} // namespace cmake_query
