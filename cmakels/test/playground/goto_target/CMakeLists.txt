cmake_minimum_required(VERSION 3.12.4)

project(sample LANGUAGES CXX)

add_library(test_lib_interface main.cc)
add_library(test_lib_private main.cc)
add_library(test_lib_public main.cc)

add_executable(sample main.cc ${CMAKE_CURRENT_SOURCE_DIR}/another_file.cc)
target_include_directories(sample PRIVATE .)
target_link_libraries(sample INTERFACE test_lib_interface)
target_link_libraries(sample PUBLIC test_lib_public)
target_link_libraries(sample PRIVATE test_lib_private)
add_subdirectory(subfolder)
