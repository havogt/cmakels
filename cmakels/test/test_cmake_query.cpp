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

namespace cmake_query {
namespace {

// TEST(cmake, instantiate) {
//   std::string root_dir =
//       "file:///workspace/cmakels/cmakels/test/playground/goto_target";
//   auto cm = instantiate_cmake(root_dir);
// }
TEST(cmake, instantiate) {
  cmake my_cmake{cmake::RoleProject, cmState::Project};

  fs::path root_dir_{"/workspace/cmakels/cmakels/test/playground/goto_target"};
  cmSystemTools::EnsureStdPipes();
  // cmsys::Encoding::CommandLineArguments encoding_args =
  //     cmsys::Encoding::CommandLineArguments::Main(argc, argv);
  // argc = encoding_args.argc();
  // argv = encoding_args.argv();

  cmSystemTools::InitializeLibUV();

  // TODO the following is a hack for the weird global state that CMake needs to
  // initialize, probably we should avoid using FindCMakeResources and try to
  // initialize the relevant parts by hand
  fs::path cmakels_dir(whereami::getExecutablePath().c_str());
  fs::path cmake_exe_in_build_tree =
      cmakels_dir.parent_path().parent_path() / "external/cmake/bin/cmake";
  fs::path cmake_exe_in_install_tree =
      cmakels_dir.parent_path().parent_path() / "bin/cmake";

  // string().c_str() to convert path to const char* on win, see
  // https://stackoverflow.com/a/54109263/5085250
  if (fs::exists(cmake_exe_in_build_tree))
    cmSystemTools::FindCMakeResources(cmake_exe_in_build_tree.string().c_str());
  else if (fs::exists(cmake_exe_in_install_tree))
    cmSystemTools::FindCMakeResources(cmake_exe_in_build_tree.string().c_str());
  else
    throw std::runtime_error("Couldn't find CMake resources.");

  my_cmake.SetHomeDirectory(root_dir_.string());

  my_cmake.SetHomeOutputDirectory(root_dir_ / "build");

  my_cmake.Run(std::vector<std::string>{}, false, true);
}

} // namespace
} // namespace cmake_query
