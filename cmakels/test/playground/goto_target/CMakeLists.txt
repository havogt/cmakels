cmake_minimum_required(VERSION 3.12.4)

project(sample LANGUAGES CXX)

add_executable(sample main.cc ${CMAKE_CURRENT_SOURCE_DIR}/another_file.cc)
target_include_directories(sample PRIVATE .)
add_subdirectory(subfolder)
