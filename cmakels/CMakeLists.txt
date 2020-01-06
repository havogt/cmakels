cmake_minimum_required(VERSION 3.12.4)

project(cmakels LANGUAGES C CXX)

option(BUILD_PLAYGROUND "" OFF)
mark_as_advanced(BUILD_PLAYGROUND)

include(CTest)

add_subdirectory(external)

include(FetchContent)
FetchContent_Declare(
	lscpp
	DOWNLOAD_COMMAND ""
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lscpp
)
FetchContent_GetProperties(lscpp)
if(NOT lscpp_POPULATED)
    FetchContent_Populate(lscpp)
    add_subdirectory(${lscpp_SOURCE_DIR} ${lscpp_BINARY_DIR})
endif()

add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
