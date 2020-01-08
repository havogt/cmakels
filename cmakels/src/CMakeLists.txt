add_library(cmake_query
    listfile_parser.cpp
    listfile_query.cpp
    cmake_query.cpp
    )

target_include_directories(cmake_query
    PUBLIC "${CMake_BINARY_DIR}/Source" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Source"
    )
target_include_directories(cmake_query
    PUBLIC "${CMake_BINARY_DIR}/Utilities" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Utilities"
    )
target_include_directories(cmake_query PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(cmake_query PRIVATE CMakeLib)
# TODO actually need to check if libstdc++ or libc++
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_libraries(cmake_query PRIVATE stdc++fs)
endif()
target_compile_features(cmake_query PUBLIC cxx_std_17)


add_executable(cmakels cmakels.cpp)
target_link_libraries(cmakels PRIVATE cmake_query)
target_link_libraries(cmakels PRIVATE lscpp)
# TODO actually need to check if libstdc++ or libc++
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_libraries(cmakels PRIVATE stdc++fs)
endif()
