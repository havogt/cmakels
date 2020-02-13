#include "core/cmake_language_server.hpp"
#include <iostream>
#include <lscpp/lsp_launcher.h>
#include <lscpp/stdio_transporter.h>

int main(int argc, char *argv[]) {
  // Redirect std::cout to std::cerr to ensure that  cmake doesn't write to
  // stdout and messes up with lsp communication.
  // IMPORTANT: we must never use std::cout for lsp communication!
  std::cout.rdbuf(std::cerr.rdbuf());

  lscpp::launch_config config;
#ifndef NDEBUG
  config.startup_delay = 0;
#endif
  config.logger = {lscpp::Verbosity_MAX, "cmakels.log"};

  if (argc < 2) {
    std::cerr << "Usage " << argv[0] << " <build-dir>\n\n"
              << "<build-dir> - build directory relative to workspace root "
                 "(usually defined by an IDE)"
              << std::endl;
    std::exit(1);
  } else {
    lscpp::launch(cmakels::cmake_language_server{argv[1]}, config,
                  lscpp::stdio_transporter{false});
  }
}