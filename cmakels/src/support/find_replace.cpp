#include <iostream>
#include <string>

namespace cmakels::support {
void replace_all(std::string &str, std::string const &find,
                 std::string const &replace) {
  size_t pos = str.find(find);
  while (pos != std::string::npos) {
    str.replace(pos, find.size(), replace);
    pos = str.find(find, pos + replace.size());
  }
}

void stream_replace_all(std::istream &in, std::ostream &out,
                        std::string const &find, std::string const &replace) {
  std::string line;

  while (getline(in, line)) {
    replace_all(line, find, replace);
    out << line;
  }
}
} // namespace cmakels::support