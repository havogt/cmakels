# The CMakeLib target from cmake itself doesn't have the include directories attached.
add_library(cmakels_CMakeLibWithIncludes INTERFACE)
target_link_libraries(cmakels_CMakeLibWithIncludes INTERFACE CMakeLib)
target_compile_definitions(cmakels_CMakeLibWithIncludes INTERFACE CMAKE_BUILD_WITH_CMAKE) #TODO private once cmakels doesn't see any cmake headers
target_include_directories(cmakels_CMakeLibWithIncludes
    INTERFACE "${CMake_BINARY_DIR}/Source" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Source"
    )
target_include_directories(cmakels_CMakeLibWithIncludes
    INTERFACE "${CMake_BINARY_DIR}/Utilities" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Utilities"
    )

add_subdirectory(support)
add_subdirectory(cmake_query)
add_subdirectory(listfile_parser)
add_subdirectory(core)

add_executable(cmakels cmakels_main.cpp)
target_link_libraries(cmakels PRIVATE lscpp)
target_link_libraries(cmakels PRIVATE core)
# TODO actually need to check if libstdc++ or libc++
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_libraries(cmakels PRIVATE stdc++fs)
endif()
