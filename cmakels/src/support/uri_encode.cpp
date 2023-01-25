#include "uri_encode.hpp"
#include "find_replace.hpp"
#include <algorithm>

namespace cmakels::support {
// TODO implement clean platform independent uri_to_filename transformations
std::string uri_to_filename(std::string const &uri) {
#ifdef _WIN32
  auto str = uri.substr(8, std::string::npos);
  std::replace(str.begin(), str.end(), '/', '\\');
  support::replace_all(str, "%3A", ":");
  return str;
#else
  return uri.substr(7, std::string::npos);
#endif
}
std::string filename_to_uri(std::string const &filename) {
#ifdef _WIN32
  auto str = filename;
  std::replace(str.begin(), str.end(), '\\', '/');
  support::replace_all(str, ":", "%3A");
  return "file:///" + str;
#else
  return "file://" + filename;
#endif
}
} // namespace cmakels::support