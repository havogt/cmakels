#pragma once

#include <iostream>
#include <string>

namespace support {
void replace_all(std::string &str, std::string const &find,
                 std::string const &replace);

// TODO is this efficient enough?
void stream_replace_all(std::istream &in, std::ostream &out,
                        std::string const &find, std::string const &replace);
} // namespace support