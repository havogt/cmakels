add_executable(startup_cmake_query startup_cmake_query.cpp)
target_link_libraries(startup_cmake_query PRIVATE cmake_query)
# TODO actually need to check if libstdc++ or libc++
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    target_link_libraries(startup_cmake_query PRIVATE stdc++fs)
endif()
