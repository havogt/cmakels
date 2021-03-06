/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#include "../support/filesystem.hpp"
#include "cmake.h"
#include <optional>

class cmMakefile;

namespace cmakels::cmake_query {

std::unique_ptr<cmake> instantiate_cmake(fs::path root_dir);

struct location {
  std::string filename;
  long line;
};

class cmake_query {
private:
  fs::path root_dir_;
  fs::path build_dir_;
  std::unique_ptr<cmake> my_cmake;

public:
  cmake_query(std::string root_dir, std::string build_dir);
  int configure() { return configure(root_dir_ / ".cmakels"); }
  int configure(fs::path const &cmake_query_build_dir);
  cmMakefile *get_makefile(std::string const &uri);
  std::optional<location> target_definition_location(std::string const &target,
                                                     std::string const &uri);
  std::optional<std::vector<std::string>>
  get_target_sources(std::string const &target, std::string const &uri);
  std::vector<std::string> get_target_names(std::string const &uri);
  std::string evaluate_variable(std::string const &name,
                                std::string const &uri);
};
} // namespace cmakels::cmake_query
