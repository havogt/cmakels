cmake_minimum_required(VERSION 3.14.0)
project(a_test_project LANGUAGES CXX)

message(STATUS "test")

set(MY_VAR "blabla")

add_subdirectory(my_subdir)
