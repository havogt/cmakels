cmake_minimum_required(VERSION 3.14.5)

project(cmakels LANGUAGES C CXX)

option(BUILD_PLAYGROUND "" OFF)
mark_as_advanced(BUILD_PLAYGROUND)

include(CTest)

set(_BUILD_TESTING ${BUILD_TESTING})
set(BUILD_TESTING OFF)
add_subdirectory(external/cmake)
set(BUILD_TESTING ${_BUILD_TESTING})
unset(_BUILD_TESTING)

add_subdirectory(../lscpp ${CMAKE_CURRENT_BINARY_DIR}/lscpp)
add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
