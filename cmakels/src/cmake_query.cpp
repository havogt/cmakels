#include "cmake_query.h"
#include <c++/7/optional>
#include <cmListFileCache.h>
#include <cmMessenger.h>
#include <fstream>

// std::optional<std::string> cmake_query::get_function_name(cmListFile const
// &lf,
//                                                           int line, int col)
//                                                           {
//   int line_1based = line + 1;

//   for (cmListFileFunction const &f : lf.Functions) {
//     if (f.Line == line_1based && col >= f.Col - f.Name.Lower.length() &&
//         col < f.EndCol) {
//       return f.Name.Lower;
//     }
//   }

//   return std::nullopt;
// }

// // TODO we need support for parsing incomplete files up to a position
// std::optional<cmListFile> cmake_query::parse(std::string const &buffer) {
//   std::string tmp_file = "/tmp/buff.cmake"; // TODO write to a better
//   location!
//   {
//     std::ofstream out(tmp_file.c_str()); // TODO use the in-memory lexer
//     instead
//                                          // (probably a bit of work)
//     out << buffer;
//   }

//   cmListFile lf;
//   {
//     auto messenger = std::make_unique<cmMessenger>();
//     // auto state = std::make_unique<cmState>();
//     // cmStateSnapshot snapshot(state.get());
//     cmListFileBacktrace lfbt{/* snapshot */};

//     if (lf.ParseFile(tmp_file.c_str(), messenger.get(), lfbt))
//       return lf;
//   }
//   return std::nullopt;
// }
