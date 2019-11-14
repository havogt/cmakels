cmake_minimum_required(VERSION 3.12.4)

project(cmakels LANGUAGES C CXX)

option(BUILD_PLAYGROUND "" OFF)
mark_as_advanced(BUILD_PLAYGROUND)

include(CTest)

add_subdirectory(external)

add_subdirectory(../lscpp ${CMAKE_CURRENT_BINARY_DIR}/lscpp)
add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
