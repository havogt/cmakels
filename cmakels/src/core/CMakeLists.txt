add_library(core
    cmake_language_server.cpp
    )

target_link_libraries(core PUBLIC cmakels_CMakeLibWithIncludes) # try to remove this dependency
target_link_libraries(core PRIVATE support)
target_link_libraries(core PUBLIC lscpp)
target_link_libraries(core PRIVATE cmake_query)
target_link_libraries(core PRIVATE listfile_parser)
target_compile_features(core PUBLIC cxx_std_17)
