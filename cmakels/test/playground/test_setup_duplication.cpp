#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace fs = std::experimental::filesystem;

class cmake_query {
public:
  cmake_query(fs::path const &home_dir,
              fs::path const &original_home_output_dir)
      : my_cmake{cmake::RoleProject, cmState::Project} {
    fs::path home_output_dir = home_dir / fs::path("tmp_build");

    // clone the cache
    fs::copy_file(original_home_output_dir / "CMakeCache.txt",
                  home_output_dir / "CMakeCache.txt",
                  fs::copy_options::overwrite_existing);

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
    std::cout << cmSystemTools::GetCMakeCommand().c_str() << std::endl;

    my_cmake.SetHomeDirectory(home_dir);
    my_cmake.SetHomeOutputDirectory(home_output_dir);

    my_cmake.Run(std::vector<std::string>{}, false);
    my_cmake.Run(std::vector<std::string>{}, false);
  }

  std::string get_value(std::string const &var) {
    auto mfs = my_cmake.GetGlobalGenerator()->GetMakefiles();
    cmMakefile *mf = mfs[0];
    return mf->GetDefinition(var);
  }

private:
  cmake my_cmake;
};

int main() {
  std::string home_dir = "/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
                         "playground/cmake_example";
  std::string original_home_output_dir =
      home_dir + "/build"; // the one from which we steal the cache

  {
    cmake_query query(home_dir, original_home_output_dir);

    std::cout << "reading MY_VAR: " << query.get_value("MY_VAR") << std::endl;
  }
}
