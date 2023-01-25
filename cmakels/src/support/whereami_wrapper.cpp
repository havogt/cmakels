#include "whereami_wrapper.hpp"

#include "whereami/whereami.h"
#include <memory>

namespace whereami {
std::string getExecutablePath() noexcept {
  int length = wai_getExecutablePath(NULL, 0, NULL);
  std::unique_ptr<char[]> path(new char[length]);
  wai_getExecutablePath(path.get(), length, NULL);
  std::string res{path.get(), static_cast<std::string::size_type>(length)};
  return res;
}
} // namespace whereami
