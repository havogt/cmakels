#include "cmake_query.hpp"

#include "../support/find_replace.hpp"
#include "../support/uri_encode.hpp"
#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "config.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace cmakels::cmake_query {

std::unique_ptr<cmake> instantiate_cmake(fs::path root_dir) {
  auto my_cmake = std::make_unique<cmake>(cmake::RoleProject, cmState::Project);

  // TODO the following is a hack for the weird global state that CMake needs
  // to initialize, probably we should avoid using FindCMakeResources and try
  // to initialize the relevant parts by hand
  fs::path cmake_exe_in_build_tree = config::cmake_exe_path;
  // TODO fix for install
  // fs::path cmake_exe_in_install_tree =
  //     cmakels_dir.parent_path().parent_path() / "bin/cmake.exe";

  // string().c_str() to convert path to const char* on win, see
  // https://stackoverflow.com/a/54109263/5085250
  if (fs::exists(cmake_exe_in_build_tree))
    cmSystemTools::FindCMakeResources(cmake_exe_in_build_tree.string().c_str());
  // else if (fs::exists(cmake_exe_in_install_tree))
  //   cmSystemTools::FindCMakeResources(cmake_exe_in_install_tree.string().c_str());
  else
    throw std::runtime_error("Couldn't find CMake resources.");

  my_cmake->SetHomeDirectory(root_dir.string());
  return my_cmake;
}

namespace {
void copy_cmake_cache(fs::path const &src, fs::path const &dst) {
  std::ifstream in(src.string());
  std::ofstream out(dst.string());
  // replace references of the original build directory with the new location
  // to suppress a warning.
  support::stream_replace_all(in, out, src.string(), dst.string());
}
} // namespace
cmake_query::cmake_query(std::string root_dir, std::string build_dir)
    : root_dir_{root_dir}, build_dir_{root_dir_ / build_dir},
      my_cmake{instantiate_cmake(fs::path{root_dir})} {}

int cmake_query::configure(fs::path const &cmake_query_build_dir) {
  fs::path cmake_cache_src{build_dir_ / "CMakeCache.txt"};
  fs::create_directories(cmake_query_build_dir);
  if (fs::exists(cmake_cache_src)) {
    auto dst = cmake_query_build_dir / "CMakeCache.txt";
    copy_cmake_cache(cmake_cache_src, dst);
  } else {
    std::cerr << "No CMakeCache.txt was found at " << cmake_cache_src.string()
              << std::endl;
  }
  my_cmake->SetHomeOutputDirectory(cmake_query_build_dir.string());

  return my_cmake->Run(std::vector<std::string>{}, false, true);
}

namespace {
// TODO check if needed
std::string normalize_filename(std::string const &filename) {
#ifdef _WIN32
  auto str = filename;
  std::replace(str.begin(), str.end(), '\\', '/');
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return str;
#else
  return filename;
#endif
}
} // namespace

cmMakefile *cmake_query::get_makefile(std::string const &uri) {
  auto mfs = my_cmake->GetGlobalGenerator()->GetMakefiles();
  auto filename = normalize_filename(support::uri_to_filename(uri));
  for (auto mf : mfs) {
    if (normalize_filename(mf->GetListFiles()[0]).compare(filename) == 0) {
      return mf;
    }
  }
  return nullptr;
}

std::optional<location> cmake_query::get_target_info(std::string const &target,
                                                     std::string const &uri) {
  auto mf = get_makefile(uri);
  if (mf) {
    auto tgt = mf->FindTargetToUse(target);
    if (tgt) {
      auto top = tgt->GetBacktrace().Top();
      return location{top.FilePath, top.Line};
    }
  }
  return std::nullopt;
}

// TODO use error type instead of optional if the target is not found
std::optional<std::vector<std::string>>
cmake_query::get_target_sources(std::string const &target,
                                std::string const &uri) {
  auto mf = get_makefile(uri);
  if (mf) {
    auto tgt = mf->FindTargetToUse(target);
    if (tgt) {
      std::vector<std::string> result;
      for (auto const &src_entry : tgt->GetSourceEntries()) {
        result.push_back(src_entry);
      }
      return result;
    }
  }
  return std::nullopt;
}

std::vector<std::string> cmake_query::get_target_names(std::string const &uri) {
  auto mf = get_makefile(uri);
  std::vector<std::string> result;
  if (mf) {
    auto &target_map = mf->GetTargets();
    for (auto const &[name, target] : target_map) {
      result.push_back(target.GetName());
    }
  }
  return result;
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
} // namespace cmakels::cmake_query
