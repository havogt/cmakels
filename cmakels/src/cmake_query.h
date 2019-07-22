#include <cmListFileCache.h>
#include <optional>
#include <string>

class cmake_query {
public:
  static std::optional<std::string>
  get_function_name(cmListFile const &, int line,
                    int col); // TODO make them free functions
  static std::optional<cmListFile> parse(std::string const &buffer);
};
