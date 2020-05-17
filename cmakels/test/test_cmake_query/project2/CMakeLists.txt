cmake_minimum_required(VERSION 3.12.4)

project(sample LANGUAGES CXX)

add_library(library1a dummy.cpp)
add_library(library1b dummy.cpp)
add_library(library2 dummy.cpp)
target_link_libraries(library2 library1a)
target_link_libraries(library2 library1b)
add_executable(a_target dummy.cpp)
target_link_libraries(a_target library2 library1a)
