#include <cmListFileCache.h>
#include <cmMessenger.h>
#include <fstream>
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace cmake_query {
// TODO we need support for parsing incomplete files up to a position
std::optional<cmListFile> parse_cmListFile(std::string const &buffer) {
  std::string tmp_file = "/tmp/buff.cmake"; // TODO write to a better location!
  {
    std::ofstream out(tmp_file.c_str()); // TODO use the in-memory lexer instead
                                         // (probably a bit of work)
    out << buffer;
  }

  cmListFile lf;
  {
    auto messenger = std::make_unique<cmMessenger>();
    // auto state = std::make_unique<cmState>();
    // cmStateSnapshot snapshot(state.get());
    cmListFileBacktrace lfbt{/* snapshot */};

    if (lf.ParseFile(tmp_file.c_str(), messenger.get(), lfbt))
      return lf;
  }
  return std::nullopt;
}

std::optional<std::string> get_function_name(cmListFile const &lf, int line,
                                             int col) {
  int line_1based = line + 1;

  for (cmListFileFunction const &f : lf.Functions) {
    if (f.Line == line_1based && col >= f.Col - f.Name.Lower.length() &&
        col < f.EndCol) {
      return f.Name.Lower;
    }
  }

  return std::nullopt;
}

std::size_t get_line(cmListFileArgument const &arg) {
  return arg.Line - 1; // convert 1-based (CMake) to 0-based (lsp)
}
std::size_t get_column(cmListFileArgument const &arg) { return arg.Col; }
std::size_t get_line_end(cmListFileArgument const &arg) {
  return arg.LineEnd - 1;
}
std::size_t get_column_end(cmListFileArgument const &arg) { return arg.ColEnd; }

std::size_t get_line(cmListFileFunction const &f) {
  return f.Line - 1; // convert 1-based (CMake) to 0-based (lsp)
}
std::size_t get_column(cmListFileFunction const &f) { return f.Col - 1; }
std::size_t get_column_functionname_end(cmListFileFunction const &f) {
  return f.Col + f.Name.Lower.length() - 1;
}
std::size_t get_line_end(cmListFileFunction const &f) {
  return f.EndLine - 1; // convert 1-based (CMake) to 0-based (lsp)
}
std::size_t get_column_end(cmListFileFunction const &f) { return f.EndCol; }
std::string get_name(cmListFileFunction const &f) { return f.Name.Original; }

namespace {

namespace test_helper {

struct range_t {
  std::size_t line_start;
  std::size_t col_start;
  std::size_t line_end;
  std::size_t col_end;
};

struct function_range_t {
  std::size_t line_start;
  std::size_t col_start;
  std::size_t col_name_end; // position of last character of the function name
  std::size_t line_end;     // position of closing parenthesis
  std::size_t col_end;
};

struct argument {
  std::string name;
  range_t range;
};

struct function {
  std::string name;
  function_range_t range;
  std::map<std::size_t, argument> arguments;

  void add_argument(std::size_t id, std::string name, range_t r) {
    arguments.insert(std::make_pair(id, argument{name, r}));
  }
};

struct listfile_test {
  std::string raw_string;
  bool valid;
  std::map<std::size_t, function> functions;

  void verify(std::optional<cmListFile> const &parsed_file) {
    ASSERT_EQ(valid, parsed_file.has_value());

    for (auto const &[id, f] : functions) {
      auto parsed_function = parsed_file->Functions[id];
      ASSERT_EQ(f.name, get_name(parsed_function));
      ASSERT_EQ(f.range.line_start, get_line(parsed_function));
      ASSERT_EQ(f.range.col_start, get_column(parsed_function));
      ASSERT_EQ(f.range.col_name_end,
                get_column_functionname_end(parsed_function));
      ASSERT_EQ(f.range.line_end, get_line_end(parsed_function));
      ASSERT_EQ(f.range.col_end, get_column_end(parsed_function));

      for (auto const &[arg_id, arg] : f.arguments) {
        auto parsed_arg = parsed_function.Arguments[arg_id];
        ASSERT_EQ(arg.name, parsed_arg.Value);

        ASSERT_EQ(arg.range.line_start, get_line(parsed_arg));
        ASSERT_EQ(arg.range.col_start, get_column(parsed_arg));
        ASSERT_EQ(arg.range.line_end, get_line_end(parsed_arg));
        ASSERT_EQ(arg.range.col_end, get_column_end(parsed_arg));
      }
    }
  }

  ~listfile_test() {
    auto result = parse_cmListFile(raw_string);
    verify(result);
  }

  void add_function(std::size_t id, std::string name, function_range_t r) {
    functions.insert(std::make_pair(id, function{name, r}));
  }
};

} // namespace test_helper

TEST(parse_cmListFile, simple) {
  test_helper::listfile_test lf = {R"?(add_subdirectory())?", true};
  lf.add_function(0, "add_subdirectory", {0, 0, 16, 0, 18});
}

TEST(parse_cmListFile, leading_whitespace) {
  test_helper::listfile_test lf = {R"?( add_subdirectory())?", true};
  lf.add_function(0, "add_subdirectory", {0, 1, 17, 0, 19});
}

TEST(parse_cmListFile, multiline) {
  test_helper::listfile_test lf = {R"?(add_subdirectory(
))?",
                                   true};
  lf.add_function(0, "add_subdirectory", {0, 0, 16, 1, 1});
}

TEST(parse_cmListFile, invalid_syntax) {
  test_helper::listfile_test lf = {R"?(add_subdirectory)?", false};
}

TEST(parse_cmListFile, with_arg) {
  test_helper::listfile_test lf = {R"?(add_subdirectory(some_folder))?", true};
  lf.add_function(0, "add_subdirectory", {0, 0, 16, 0, 29});
  lf.functions[0].add_argument(0, "some_folder", {0, 17, 0, 28});
}

TEST(parse_cmListFile, multiple_args) {
  test_helper::listfile_test lf = {R"?(function(arg0 arg1  arg2))?", true};
  lf.add_function(0, "function", {0, 0, 8, 0, 25});
  lf.functions[0].add_argument(0, "arg0", {0, 9, 0, 13});
  lf.functions[0].add_argument(1, "arg1", {0, 14, 0, 18});
  lf.functions[0].add_argument(2, "arg2", {0, 20, 0, 24});
}

TEST(parse_cmListFile, multiline_with_args_and_spaces) {
  test_helper::listfile_test lf = {R"?(target_link_libraries( a_target
    PRIVATE  some_lib))?",
                                   true};
  lf.add_function(0, "target_link_libraries", {0, 0, 21, 1, 22});
  lf.functions[0].add_argument(0, "a_target", {0, 23, 0, 31});
  lf.functions[0].add_argument(1, "PRIVATE", {1, 4, 1, 11});
  lf.functions[0].add_argument(2, "some_lib", {1, 13, 1, 21});
}
} // namespace

} // namespace cmake_query
