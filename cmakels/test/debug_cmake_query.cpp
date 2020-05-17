#include <cmake_query/cmake_query.hpp>

#include <iostream>
#include <support/filesystem.hpp>
#include <support/uri_encode.hpp>
#include <support/whereami_wrapper.hpp>

#include "config.h"

int main() {
  std::string root_dir =
      test_cmake_query_config::projects_src_dir + "/project2";

  cmakels::cmake_query::cmake_query query{root_dir, "build"};

  query.configure(test_cmake_query_config::cmakefiles_dir +
                  "/project2/.cmakels");

  std::cout << "before" << std::endl;
  auto deps = query.get_target_dependencies(
      "a_target",
      cmakels::support::filename_to_uri(root_dir) + "/CMakeLists.txt");
  std::cout << "after" << std::endl;

  for (auto const &dep : *deps) {
    std::cout << dep.first << std::endl;
  }
}
