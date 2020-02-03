/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <gtest/gtest.h>

#include <cmake_query.hpp>

#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"

#include <support/filesystem.hpp>
#include <support/whereami_wrapper.hpp>

#include "config.h"

TEST(cmake_query, instantiate_cmake) {
  std::string root_dir =
      test_cmake_query_config::projects_src_dir + "/project1";
  auto cm = cmake_query::instantiate_cmake(root_dir);
}

TEST(cmake_query, configure) {
  std::string root_dir =
      test_cmake_query_config::projects_src_dir + "/project1";

  cmake_query::cmake_query query{root_dir, "build"};
  ASSERT_EQ(0, query.configure(test_cmake_query_config::cmakefiles_dir +
                               "/project1/.cmakels"));
}
