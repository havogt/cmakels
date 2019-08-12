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
      : root_dir_{root_dir}, build_dir_{root_dir_ / build_dir},
        my_cmake{cmake::RoleProject, cmState::Project} {

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

  cmMakefile *get_makefile(std::string const &uri) {
    auto mfs = my_cmake.GetGlobalGenerator()->GetMakefiles();
    for (auto mf : mfs) {
      if (("file://" + mf->GetListFiles()[0]).compare(uri) ==
          0) { // TODO fix file://
        return mf;
      }
    }
    return nullptr;
  }

  bool is_target(std::string const &target, std::string const &uri) {
    auto mf = get_makefile(uri);
    return mf && mf->FindTargetToUse(target);
  }

  std::string evaluate_variable(std::string const &name,
                                std::string const &uri) {
    auto mf = get_makefile(uri);
    if (mf) {
      auto var = mf->GetDefinition(name);
      return var ? var : "tried file" + mf->GetListFiles()[0];
    }
    return "<variable-not-found>";
  }
};
} // namespace cmake_query
