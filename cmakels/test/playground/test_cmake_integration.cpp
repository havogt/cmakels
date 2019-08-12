#include "cmGlobalGenerator.h"
#include "cmListFileCache.h"
#include "cmMakefile.h"
#include "cmMessenger.h"
#include "cmState.h"
#include "cmStateSnapshot.h"
#include "cmSystemTools.h"
#include "cmake.h"
#include <fstream>
#include <iostream>
#include <memory>

class cmake_query {
public:
  cmake_query() : my_cmake{cmake::RoleProject, cmState::Project} {
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

    my_cmake.SetHomeDirectory(
        "/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
        "playground/cmake_example");
    my_cmake.SetHomeOutputDirectory(
        "/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
        "playground/cmake_example/build");

    my_cmake.Run(std::vector<std::string>{}, false, true);
  }

  // std::string get_function(std::string filename, int line, int col) const {
  //   auto mfs =
  //       my_cmake.GetGlobalGenerator()
  //           ->GetMakefiles(); // GetCurrentMakefile(); //->GetListFiles();

  //   for (auto const &l : mfs[0]->GetListFiles())
  //     std::cout << l << std::endl;

  //   std::cout << "----------------" << std::endl;

  //   for (cmMakefile *mf : mfs) {
  //     for (cmListFile const &lf : mf->GetParsedListFiles()) {
  //       std::cout << lf.Filename << std::endl;
  //       if (lf.Filename.compare(filename) == 0) {
  //         std::cout << "Found the file" << std::endl;
  //         for (cmListFileFunction const &f : lf.Functions) {
  //           std::cout << f.Name.Lower << " - " << f.Line << " - " << f.Col
  //                     << " - " << f.EndCol << std::endl;
  //           if (f.Line == line && col >= f.Col - f.Name.Lower.length() &&
  //               col < f.EndCol) {
  //             // std::cout << "Found the function" << std::endl;
  //             std::cout << "Found " << f.Name.Lower << std::endl;
  //           }
  //           // std::cout << lf.Filename << ":" << f.Line << " ("
  //           //           << f.Name.Original << ")" << std::endl;
  //         }
  //       }
  //     }
  //   }
  //   std::cout << "done..." << std::endl;
  //   return "";
  // }

  std::string get_value(std::string const &var) {
    auto mfs = my_cmake.GetGlobalGenerator()->GetMakefiles();

    for (auto const &l : mfs[0]->GetListFiles())
      std::cout << l << std::endl;

    // std::cout << "----------------" << std::endl;

    cmMakefile *mf = mfs[0];
    return mf->GetDefinition(var);
  }

  std::string get_function_from_buffer(std::string const &buffer, int line,
                                       int col) const {
    std::string tmp_file = "/tmp/buff.cmake";
    {
      std::ofstream out(tmp_file.c_str());
      out << buffer;
    }

    cmListFile lf;
    {
      auto messenger = std::make_unique<cmMessenger>();
      // auto state = std::make_unique<cmState>();
      // cmStateSnapshot snapshot(state.get());
      cmListFileBacktrace lfbt{/* snapshot */};

      lf.ParseFile(tmp_file.c_str(), messenger.get(), lfbt);
    }

    for (cmListFileFunction const &f : lf.Functions) {
      std::cout << f.Name.Lower << " - " << f.Line << " - "
                << f.Col - f.Name.Lower.length() << " - " << f.EndCol
                << std::endl;
      if (f.Line == line && col >= f.Col - f.Name.Lower.length() &&
          col < f.EndCol) {
        std::cout << "Found " << f.Name.Lower << std::endl;
        return f.Name.Lower;
      }
    }

    return "";
  }

private:
  cmake my_cmake;
};

int main() {
  // cmSystemTools::InitializeLibUV();

  // auto wd =
  //     cmSystemTools::GetCurrentWorkingDirectory(); // otherwise segfault...

  // std::cout << "cwd: " << wd << std::endl;

  // cmSystemTools::FindCMakeResources((wd + "/cmakels").c_str());

  // cmake my_cmake{cmake::RoleProject, cmState::Project};

  // // cmake hcm(cmake::RoleInternal, cmState::Unknown);
  // my_cmake.SetHomeDirectory("/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
  //                           "playground/cmake_example");
  // my_cmake.SetHomeOutputDirectory(
  //     "/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
  //     "playground/cmake_example/build");

  // std::cout << cmSystemTools::GetCMakeCommand().c_str() << std::endl;

  // my_cmake.AddCMakePaths();

  // my_cmake.Run(std::vector<std::string>{});

  // --------------------------------------------------------
  // cmSystemTools::EnsureStdPipes();
  // // cmsys::Encoding::CommandLineArguments encoding_args =
  // //     cmsys::Encoding::CommandLineArguments::Main(argc, argv);
  // // argc = encoding_args.argc();
  // // argv = encoding_args.argv();

  // cmSystemTools::InitializeLibUV();

  // auto wd = cmSystemTools::GetCurrentWorkingDirectory();
  // cmSystemTools::FindCMakeResources(
  //     "/home/vogtha/lsp/experiments/cmakels/"
  //     "cmakels/build/code/external/cmake/bin/cmake");
  // // cmSystemTools::FindCMakeResources((wd + "/cmakels").c_str());
  // std::cout << cmSystemTools::GetCMakeCommand().c_str() << std::endl;

  // cmake my_cmake{cmake::RoleProject, cmState::Project};

  // // cmake hcm(cmake::RoleInternal, cmState::Unknown);
  // my_cmake.SetHomeDirectory("/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
  //                           "playground/cmake_example");
  // my_cmake.SetHomeOutputDirectory(
  //     "/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
  //     "playground/cmake_example/build");

  // my_cmake.Run(std::vector<std::string>{}, false);

  // // my_cmake.GetProperty(const std::string &prop)

  // // my_cmake.Configure();

  // auto mfs = my_cmake.GetGlobalGenerator()
  //                ->GetMakefiles(); // GetCurrentMakefile();
  //                //->GetListFiles();

  // // if (!curMakefile)
  // //   std::cout << "oh no" << std::endl;

  // for (auto const &l : mfs[0]->GetListFiles())
  //   std::cout << l << std::endl;

  // // for (auto const &l : mfs[0]->GetParsedListFiles())
  // //   std::cout << l.Filename << std::endl;

  // // auto res = mfs[0]->GetDefinition("MY_VAR");
  // // std::cout << res << std::endl;
  // // std::cout << "here" << std::endl;
  // // std::string res2 = mfs[0]->GetDefinition("another_var");
  // // std::cout << "gone" << std::endl;
  // // std::cout << res2 << std::endl;
  // // std::cout << "gone2" << std::endl;

  // cmMakefile &mf = *(mfs[0]);

  // for (cmMakefile *mf : mfs) {
  //   for (cmListFile const &lf : mf->GetParsedListFiles()) {
  //     // std::cout << lf.Filename << std::endl;
  //     for (cmListFileFunction const &f : lf.Functions)
  //       std::cout << lf.Filename << ":" << f.Line << " (" <<
  //       f.Name.Original
  //                 << ")" << std::endl;
  //   }
  // }

  // my_cmake.Generate();

  cmake_query query;
  // query.get_function("/home/vogtha/lsp/experiments/cmakels/cmakels/test/"
  //                    "playground/cmake_example/my_subdir/CMakeLists.txt",
  //                    1, 1);

  // std::string a_cmake_file = "cmake_minimum_required(VERSION 3.14.0)\n"
  //                            "project(a_test_project LANGUAGES CXX)\n\n"
  //                            "message(STATUS \"test\")\n\n"
  //                            "set(MY_VAR \"blabla\")\n\n"
  //                            "add_subdirectory(my_subdir)\n";

  // query.get_function_from_buffer(a_cmake_file, 1, 6);

  std::cout << "reading MY_VAR: " << query.get_value("MY_VAR") << std::endl;
}
