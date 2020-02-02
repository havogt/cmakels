/*
 * Copyright 2019-2020 Hannes Vogt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
// #include <gtest/gtest.h>

#define CMAKE_BUILD_WITH_CMAKE

#include <cmake_query.hpp>

#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cm_uv.h"
#include "cmsys/Encoding.hxx"

#ifdef CMAKE_BUILD_WITH_CMAKE
#include "cmDocumentation.h"
#include "cmDynamicLoader.h"
#endif

#include <iostream>
#include <support/filesystem.hpp>
#include <support/whereami_wrapper.hpp>

#if 0
#ifdef CMAKE_BUILD_WITH_CMAKE
static const char *cmDocumentationName[][2] = {
    {nullptr, "  cmake - Cross-Platform Makefile Generator."},
    {nullptr, nullptr}};

static const char *cmDocumentationUsage[][2] = {
    {nullptr, "  cmake [options] <path-to-source>\n"
              "  cmake [options] <path-to-existing-build>\n"
              "  cmake [options] -S <path-to-source> -B <path-to-build>"},
    {nullptr, "Specify a source directory to (re-)generate a build system for "
              "it in the current working directory.  Specify an existing build "
              "directory to re-generate its build system."},
    {nullptr, nullptr}};

static const char *cmDocumentationUsageNote[][2] = {
    {nullptr, "Run 'cmake --help' for more information."}, {nullptr, nullptr}};

#define CMAKE_BUILD_OPTIONS                                                    \
  "  <dir>          = Project binary directory to be built.\n"                 \
  "  --parallel [<jobs>], -j [<jobs>]\n"                                       \
  "                 = Build in parallel using the given number of jobs. \n"    \
  "                   If <jobs> is omitted the native build tool's \n"         \
  "                   default number is used.\n"                               \
  "                   The CMAKE_BUILD_PARALLEL_LEVEL environment "             \
  "variable\n"                                                                 \
  "                   specifies a default parallel level when this "           \
  "option\n"                                                                   \
  "                   is not given.\n"                                         \
  "  --target <tgt>..., -t <tgt>... \n"                                        \
  "                 = Build <tgt> instead of default targets.\n"               \
  "  --config <cfg> = For multi-configuration tools, choose <cfg>.\n"          \
  "  --clean-first  = Build target 'clean' first, then build.\n"               \
  "                   (To clean only, use --target 'clean'.)\n"                \
  "  --verbose, -v  = Enable verbose output - if supported - including\n"      \
  "                   the build commands to be executed. \n"                   \
  "  --             = Pass remaining options to the native tool.\n"

#define CMAKE_INSTALL_OPTIONS                                                  \
  "  <dir>              = Project binary directory to install.\n"              \
  "  --config <cfg>     = For multi-configuration tools, choose <cfg>.\n"      \
  "  --component <comp> = Component-based install. Only install <comp>.\n"     \
  "  --prefix <prefix>  = The installation prefix CMAKE_INSTALL_PREFIX.\n"     \
  "  --strip            = Performing install/strip.\n"                         \
  "  -v --verbose       = Enable verbose output.\n"

static const char *cmDocumentationOptions[][2] = {
    CMAKE_STANDARD_OPTIONS_TABLE,
    {"-E", "CMake command mode."},
    {"-L[A][H]", "List non-advanced cached variables."},
    {"--build <dir>", "Build a CMake-generated project binary tree."},
    {"--install <dir>", "Install a CMake-generated project binary tree."},
    {"--open <dir>", "Open generated project in the associated application."},
    {"-N", "View mode only."},
    {"-P <file>", "Process script mode."},
    {"--find-package", "Run in pkg-config like mode."},
    {"--graphviz=[file]", "Generate graphviz of dependencies, see "
                          "CMakeGraphVizOptions.cmake for more."},
    {"--system-information [file]", "Dump information about this system."},
    {"--loglevel=<error|warn|notice|status|verbose|debug|trace>",
     "Set the verbosity of messages from CMake files."},
    {"--debug-trycompile", "Do not delete the try_compile build tree. Only "
                           "useful on one try_compile at a time."},
    {"--debug-output", "Put cmake in a debug mode."},
    {"--trace", "Put cmake in trace mode."},
    {"--trace-expand", "Put cmake in trace mode with variable expansion."},
    {"--trace-source=<file>",
     "Trace only this CMake file/module. Multiple options allowed."},
    {"--warn-uninitialized", "Warn about uninitialized values."},
    {"--warn-unused-vars", "Warn about unused variables."},
    {"--no-warn-unused-cli", "Don't warn about command line options."},
    {"--check-system-vars", "Find problems with variable usage in system "
                            "files."},
    {nullptr, nullptr}};

#endif

static cmMakefile *cmakemainGetMakefile(cmake *cm) {
  if (cm && cm->GetDebugOutput()) {
    cmGlobalGenerator *gg = cm->GetGlobalGenerator();
    if (gg) {
      return gg->GetCurrentMakefile();
    }
  }
  return nullptr;
}

static std::string cmakemainGetStack(cmake *cm) {
  std::string msg;
  cmMakefile *mf = cmakemainGetMakefile(cm);
  if (mf) {
    msg = mf->FormatListFileStack();
    if (!msg.empty()) {
      msg = "\n   Called from: " + msg;
    }
  }

  return msg;
}

static void cmakemainMessageCallback(const std::string &m,
                                     const char * /*unused*/, cmake *cm) {
  std::cerr << m << cmakemainGetStack(cm) << std::endl << std::flush;
}

static void cmakemainProgressCallback(const std::string &m, float prog,
                                      cmake *cm) {
  cmMakefile *mf = cmakemainGetMakefile(cm);
  std::string dir;
  if (mf && cmHasLiteralPrefix(m, "Configuring") && (prog < 0)) {
    dir = " ";
    dir += mf->GetCurrentSourceDirectory();
  } else if (mf && cmHasLiteralPrefix(m, "Generating")) {
    dir = " ";
    dir += mf->GetCurrentBinaryDirectory();
  }

  if ((prog < 0) || (!dir.empty())) {
    std::cout << "-- " << m << dir << cmakemainGetStack(cm) << std::endl;
  }

  std::cout.flush();
}

int do_cmake(int ac, char const *const *av) {
  if (cmSystemTools::GetCurrentWorkingDirectory().empty()) {
    std::cerr << "Current working directory cannot be established."
              << std::endl;
    return 1;
  }

#ifdef CMAKE_BUILD_WITH_CMAKE
  cmDocumentation doc;
  doc.addCMakeStandardDocSections();
  if (doc.CheckOptions(ac, av)) {
    // Construct and print requested documentation.
    cmake hcm(cmake::RoleInternal, cmState::Unknown);
    hcm.SetHomeDirectory("");
    hcm.SetHomeOutputDirectory("");
    hcm.AddCMakePaths();

    // the command line args are processed here so that you can do
    // -DCMAKE_MODULE_PATH=/some/path and have this value accessible here
    std::vector<std::string> args(av, av + ac);
    hcm.SetCacheArgs(args);

    auto generators = hcm.GetGeneratorsDocumentation();

    doc.SetName("cmake");
    doc.SetSection("Name", cmDocumentationName);
    doc.SetSection("Usage", cmDocumentationUsage);
    if (ac == 1) {
      doc.AppendSection("Usage", cmDocumentationUsageNote);
    }
    doc.AppendSection("Generators", generators);
    doc.PrependSection("Options", cmDocumentationOptions);

    return doc.PrintRequestedDocumentation(std::cout) ? 0 : 1;
  }
#else
  if (ac == 1) {
    std::cout
        << "Bootstrap CMake should not be used outside CMake build process."
        << std::endl;
    return 0;
  }
#endif

  bool sysinfo = false;
  bool list_cached = false;
  bool list_all_cached = false;
  bool list_help = false;
  bool view_only = false;
  cmake::WorkingMode workingMode = cmake::NORMAL_MODE;
  std::vector<std::string> args;
  for (int i = 0; i < ac; ++i) {
    if (strcmp(av[i], "-i") == 0) {
      /* clang-format off */
      std::cerr <<
        "The \"cmake -i\" wizard mode is no longer supported.\n"
        "Use the -D option to set cache values on the command line.\n"
        "Use cmake-gui or ccmake for an interactive dialog.\n";
      /* clang-format on */
      return 1;
    }
    if (strcmp(av[i], "--system-information") == 0) {
      sysinfo = true;
    } else if (strcmp(av[i], "-N") == 0) {
      view_only = true;
    } else if (strcmp(av[i], "-L") == 0) {
      list_cached = true;
    } else if (strcmp(av[i], "-LA") == 0) {
      list_all_cached = true;
    } else if (strcmp(av[i], "-LH") == 0) {
      list_cached = true;
      list_help = true;
    } else if (strcmp(av[i], "-LAH") == 0) {
      list_all_cached = true;
      list_help = true;
    } else if (cmHasLiteralPrefix(av[i], "-P")) {
      if (i == ac - 1) {
        cmSystemTools::Error("No script specified for argument -P");
        return 1;
      }
      workingMode = cmake::SCRIPT_MODE;
      args.emplace_back(av[i]);
      i++;
      args.emplace_back(av[i]);
    } else if (cmHasLiteralPrefix(av[i], "--find-package")) {
      workingMode = cmake::FIND_PACKAGE_MODE;
      args.emplace_back(av[i]);
    } else {
      args.emplace_back(av[i]);
    }
  }
  if (sysinfo) {
    cmake cm(cmake::RoleProject, cmState::Project);
    cm.SetHomeDirectory("");
    cm.SetHomeOutputDirectory("");
    int ret = cm.GetSystemInformation(args);
    return ret;
  }
  cmake::Role const role = workingMode == cmake::SCRIPT_MODE
                               ? cmake::RoleScript
                               : cmake::RoleProject;
  cmState::Mode mode = cmState::Unknown;
  switch (workingMode) {
  case cmake::NORMAL_MODE:
    mode = cmState::Project;
    break;
  case cmake::SCRIPT_MODE:
    mode = cmState::Script;
    break;
  case cmake::FIND_PACKAGE_MODE:
    mode = cmState::FindPackage;
    break;
  }
  cmake cm(role, mode);
  cm.SetHomeDirectory("");
  cm.SetHomeOutputDirectory("");
  cmSystemTools::SetMessageCallback(
      [&cm](const std::string &msg, const char *title) {
        cmakemainMessageCallback(msg, title, &cm);
      });
  cm.SetProgressCallback([&cm](const std::string &msg, float prog) {
    cmakemainProgressCallback(msg, prog, &cm);
  });
  cm.SetWorkingMode(workingMode);

  int res = cm.Run(args, view_only);
  if (list_cached || list_all_cached) {
    std::cout << "-- Cache values" << std::endl;
    std::vector<std::string> keys = cm.GetState()->GetCacheEntryKeys();
    for (std::string const &k : keys) {
      cmStateEnums::CacheEntryType t = cm.GetState()->GetCacheEntryType(k);
      if (t != cmStateEnums::INTERNAL && t != cmStateEnums::STATIC &&
          t != cmStateEnums::UNINITIALIZED) {
        const char *advancedProp =
            cm.GetState()->GetCacheEntryProperty(k, "ADVANCED");
        if (list_all_cached || !advancedProp) {
          if (list_help) {
            std::cout << "// "
                      << cm.GetState()->GetCacheEntryProperty(k, "HELPSTRING")
                      << std::endl;
          }
          std::cout << k << ":" << cmState::CacheEntryTypeToString(t) << "="
                    << cm.GetState()->GetCacheEntryValue(k) << std::endl;
          if (list_help) {
            std::cout << std::endl;
          }
        }
      }
    }
  }

  // Always return a non-negative value.  Windows tools do not always
  // interpret negative return values as errors.
  if (res != 0) {
    return 1;
  }
  return 0;
}

int main(int ac, char const *const *av) {
  cmSystemTools::EnsureStdPipes();
  cmsys::Encoding::CommandLineArguments args =
      cmsys::Encoding::CommandLineArguments::Main(ac, av);
  ac = args.argc();
  av = args.argv();

  cmSystemTools::EnableMSVCDebugHook();
  cmSystemTools::InitializeLibUV();
  cmSystemTools::FindCMakeResources(av[0]);
  int ret = do_cmake(ac, av);
#ifdef CMAKE_BUILD_WITH_CMAKE
  cmDynamicLoader::FlushCache();
#endif
  uv_loop_close(uv_default_loop());
  return ret;
}
#endif

#if 1
int main() {
  std::cout << "test" << std::endl;
  cmake_query::cmake_query query{"/home/vogtha/lsp/experiments/cmakels/cmakels/"
                                 "test/playground/goto_target",
                                 "build"};
}
#endif

#if 0
int main() {
  cmake my_cmake{cmake::RoleProject, cmState::Project};

  fs::path root_dir_{"/home/vogtha/lsp/experiments/cmakels/cmakels/test/playground/goto_target"};
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
      cmakels_dir.parent_path() / "external/cmake/bin/cmake";
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

  return 0;
}

#endif
