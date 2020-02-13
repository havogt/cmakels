/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <cmListFileCache.h>
#include <optional>
#include <string>

namespace cmakels::parser {
using listfile_t = cmListFile;
using listfile_function_t = cmListFileFunction;
using listfile_argument_t = cmListFileArgument;
using listfile_functions_t = decltype(cmListFile::Functions);
using listfile_arguments_t = decltype(cmListFileFunction::Arguments);

// TODO we need support for parsing incomplete files up to a position
std::optional<listfile_t> parse_listfile(
    std::string const &buffer); // TODO probably throw instead of optional

// std::optional<std::string> get_function_name(cmListFile const &lf, int line,
//                                              int col);

listfile_functions_t get_functions(listfile_t const &f);
listfile_arguments_t get_arguments(listfile_function_t const &f);

std::size_t get_line(listfile_function_t const &f);
std::size_t get_column(listfile_function_t const &f);
std::size_t get_column_functionname_end(listfile_function_t const &f);
std::size_t get_line_end(listfile_function_t const &f);
std::size_t get_column_end(listfile_function_t const &f);
std::string get_name(listfile_function_t const &f);

std::size_t get_line(listfile_argument_t const &arg);
std::size_t get_column(listfile_argument_t const &arg);
std::size_t get_line_end(listfile_argument_t const &arg);
std::size_t get_column_end(listfile_argument_t const &arg);
std::string get_name(listfile_argument_t const &f);
} // namespace cmakels::parser
