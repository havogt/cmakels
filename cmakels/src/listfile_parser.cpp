#include "listfile_parser.hpp"

#include <cmMessenger.h>
#include <fstream>

namespace cmake_query {
// TODO we need support for parsing incomplete files up to a position
std::optional<cmListFile>
parse_listfile(std::string const
                   &buffer) { // TODO be able to parse from file or from buffer
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

// std::optional<std::string> get_function_name(cmListFile const &lf, int line,
//                                              int col) {
//   int line_1based = line + 1;

//   for (cmListFileFunction const &f : lf.Functions) {
//     if (f.Line == line_1based && col >= f.Col - f.Name.Lower.length() &&
//         col < f.EndCol) {
//       return f.Name.Lower;
//     }
//   }

//   return std::nullopt;
// }

std::size_t get_line(cmListFileArgument const &arg) {
  return arg.Line - 1; // convert 1-based (CMake) to 0-based (lsp)
}
std::size_t get_column(cmListFileArgument const &arg) { return arg.Col; }
std::size_t get_line_end(cmListFileArgument const &arg) {
  return arg.LineEnd - 1;
}
std::size_t get_column_end(cmListFileArgument const &arg) { return arg.ColEnd; }
std::string get_name(cmListFileArgument const &arg) { return arg.Value; }

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
} // namespace cmake_query
