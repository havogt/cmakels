cmake_minimum_required(VERSION 3.12.4)
project(loguru LANGUAGES CXX)

find_package(Threads)

add_library(loguru loguru.cpp)
target_include_directories(loguru PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(loguru PRIVATE Threads::Threads)
if(CMAKE_DL_LIBS)
    target_link_libraries(loguru PRIVATE ${CMAKE_DL_LIBS})
endif()
