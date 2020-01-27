#include "cmake_query.hpp"

#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "support/whereami_wrapper.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace cmake_query {

cmake_query::cmake_query(std::string root_dir, std::string build_dir)
    : root_dir_{root_dir}, build_dir_{root_dir_ / build_dir},
      my_cmake{cmake::RoleProject, cmState::Project} {

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

  if (fs::exists(cmake_exe_in_build_tree))
    cmSystemTools::FindCMakeResources(cmake_exe_in_build_tree.c_str());
  else if (fs::exists(cmake_exe_in_install_tree))
    cmSystemTools::FindCMakeResources(cmake_exe_in_build_tree.c_str());
  else
    throw std::runtime_error("Couldn't find CMake resources.");

  my_cmake.SetHomeDirectory(root_dir_.string());
}

void cmake_query::configure() {
  fs::path cmake_query_build_dir = root_dir_ / ".cmakels";
  fs::create_directories(cmake_query_build_dir);
  fs::path cmake_cache_src = build_dir_ / "CMakeCache.txt";
  if (fs::exists(cmake_cache_src))
    fs::copy_file(cmake_cache_src, cmake_query_build_dir / "CMakeCache.txt",
                  fs::copy_options::overwrite_existing);
  else {
    std::cerr << "No CMakeCache.txt was found." << std::endl;
  }
  my_cmake.SetHomeOutputDirectory(cmake_query_build_dir.string());

  my_cmake.Run(std::vector<std::string>{}, false, true);
}

cmMakefile *cmake_query::get_makefile(std::string const &uri) {
  auto mfs = my_cmake.GetGlobalGenerator()->GetMakefiles();
  for (auto mf : mfs) {
    if (("file://" + mf->GetListFiles()[0]).compare(uri) ==
        0) { // TODO fix file://
      return mf;
    }
  }
  return nullptr;
}

bool cmake_query::is_target(std::string const &target, std::string const &uri) {
  auto mf = get_makefile(uri);
  return mf && mf->FindTargetToUse(target);
}

std::string cmake_query::evaluate_variable(std::string const &name,
                                           std::string const &uri) {
  auto mf = get_makefile(uri);
  if (mf) {
    auto var = mf->GetDefinition(name);
    return var ? var : "tried file" + mf->GetListFiles()[0];
  }
  return "<variable-not-found>";
}
} // namespace cmake_query
