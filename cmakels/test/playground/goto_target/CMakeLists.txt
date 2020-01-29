cmake_minimum_required(VERSION 3.12.4)

project(sample LANGUAGES CXX)

add_executable(sample main.cc)
add_subdirectory(subfolder)
