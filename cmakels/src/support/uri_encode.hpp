#pragma once

#include <string>

namespace cmakels::support {
std::string uri_to_filename(std::string const &uri);
std::string filename_to_uri(std::string const &filename);
} // namespace cmakels::support
