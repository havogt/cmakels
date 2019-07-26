add_library(cmake_query listfile_parser.cpp)

target_include_directories(cmake_query
    PUBLIC "${CMake_BINARY_DIR}/Source" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Source"
    )
target_include_directories(cmake_query
    PUBLIC "${CMake_BINARY_DIR}/Utilities" # needs to be private for encapsulation!
            "${CMake_SOURCE_DIR}/Utilities"
    )
target_include_directories(cmake_query PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(cmake_query PUBLIC /home/blabla)
target_link_libraries(cmake_query PRIVATE CMakeLib)
target_compile_features(cmake_query PUBLIC cxx_std_17) # PUBLIC: header contains optional... should be ensured by subprojects???


add_executable(cmakels cmakels.cpp)
target_link_libraries(cmakels PRIVATE cmake_query)
target_link_libraries(cmakels PRIVATE lscpp)
