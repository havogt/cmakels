#pragma once

#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include <experimental/filesystem>
#include <iostream>

namespace cmake_query {

namespace fs = std::experimental::filesystem;

class cmake_query {
private:
  fs::path root_dir_;
  fs::path build_dir_;
  cmake my_cmake;

public:
  cmake_query(std::string root_dir, std::string build_dir)
      : root_dir_{"/home/vogtha/git/stella_old/stella"}, // TODO fix root_dir
        build_dir_{root_dir_ / build_dir}, my_cmake{cmake::RoleProject,
                                                    cmState::Project} {

    cmSystemTools::EnsureStdPipes();
    // cmsys::Encoding::CommandLineArguments encoding_args =
    //     cmsys::Encoding::CommandLineArguments::Main(argc, argv);
    // argc = encoding_args.argc();
    // argv = encoding_args.argv();

    cmSystemTools::InitializeLibUV();

    auto wd = cmSystemTools::GetCurrentWorkingDirectory();
    cmSystemTools::FindCMakeResources(
        "/home/vogtha/lsp/experiments/cmakels/"
        "cmakels/build/code/external/cmake/bin/cmake");
    // cmSystemTools::FindCMakeResources((wd + "/cmakels").c_str());
    // std::cout << cmSystemTools::GetCMakeCommand().c_str() << std::endl;

    my_cmake.SetHomeDirectory(root_dir_);
  }

  void configure() {
    fs::path cmake_query_build_dir = root_dir_ / ".cmakels";
    fs::create_directories(cmake_query_build_dir);
    fs::copy_file(build_dir_ / "CMakeCache.txt",
                  cmake_query_build_dir / "CMakeCache.txt",
                  fs::copy_options::overwrite_existing);

    my_cmake.SetHomeOutputDirectory(cmake_query_build_dir);

    my_cmake.Run(std::vector<std::string>{}, false, true);
  }

  std::string evaluate_variable(std::string name) {
    auto mfs = my_cmake.GetGlobalGenerator()->GetMakefiles();
    cmMakefile *mf = mfs[0];
    auto var = mf->GetDefinition(name);
    return var ? var : "variable not found!";
  }
};
} // namespace cmake_query
