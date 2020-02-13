
# Need one file per configuration for multi-config generators (Visual Studio generator),
# otherwise the file gets written multiple times with different result which is illegal.
file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/$<CONFIG>/config.h
    INPUT config.h.in
)

add_library(cmake_query
    cmake_query.cpp
    )

target_link_libraries(cmake_query PUBLIC cmakels_CMakeLibWithIncludes) # TODO private once cmakels doesn't see any cmake headers
target_include_directories(cmake_query PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(cmake_query PRIVATE ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/$<CONFIG>)
target_link_libraries(cmake_query PRIVATE support)
# TODO actually need to check if libstdc++ or libc++
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_libraries(cmake_query PRIVATE stdc++fs)
endif()
target_compile_features(cmake_query PUBLIC cxx_std_17)