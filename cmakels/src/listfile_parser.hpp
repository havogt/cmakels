#include <cmListFileCache.h>
#include <optional>
#include <string>

namespace cmake_query {
using listfile_t = cmListFile;
using listfile_function_t = cmListFileFunction;
using listfile_argument_t = cmListFileArgument;

// TODO we need support for parsing incomplete files up to a position
std::optional<listfile_t> parse_listfile(
    std::string const &buffer); // TODO probably throw instead of optional

// std::optional<std::string> get_function_name(cmListFile const &lf, int line,
//                                              int col);

std::size_t get_line(listfile_argument_t const &arg);
std::size_t get_column(listfile_argument_t const &arg);
std::size_t get_line_end(listfile_argument_t const &arg);
std::size_t get_column_end(listfile_argument_t const &arg);
std::string get_name(listfile_argument_t const &f);

std::size_t get_line(listfile_function_t const &f);
std::size_t get_column(listfile_function_t const &f);
std::size_t get_column_functionname_end(listfile_function_t const &f);
std::size_t get_line_end(listfile_function_t const &f);
std::size_t get_column_end(listfile_function_t const &f);
std::string get_name(listfile_function_t const &f);
} // namespace cmake_query
