#pragma once

#include "cmake.h"
#include "support/filesystem.hpp"

class cmMakefile;

namespace cmake_query {

class cmake_query {
private:
  fs::path root_dir_;
  fs::path build_dir_;
  cmake my_cmake;

public:
  cmake_query(std::string root_dir, std::string build_dir);
  void configure();
  cmMakefile *get_makefile(std::string const &uri);
  bool is_target(std::string const &target, std::string const &uri);
  std::string evaluate_variable(std::string const &name,
                                std::string const &uri);
};
} // namespace cmake_query
