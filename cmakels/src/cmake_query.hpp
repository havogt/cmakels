/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "cmake.h"
#include "support/filesystem.hpp"
#include <optional>

class cmMakefile;

namespace cmake_query {

struct location {
  std::string filename;
  long line;
};

class cmake_query {
private:
  fs::path root_dir_;
  fs::path build_dir_;
  cmake my_cmake;

public:
  cmake_query(std::string root_dir, std::string build_dir);
  void configure();
  cmMakefile *get_makefile(std::string const &uri);
  std::optional<location> get_target_info(std::string const &target,
                                          std::string const &uri);
  std::string evaluate_variable(std::string const &name,
                                std::string const &uri);
};
} // namespace cmake_query
