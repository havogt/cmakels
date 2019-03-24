cmake_minimum_required(VERSION 3.12.4)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(lscpp LANGUAGES CXX)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../install" CACHE PATH "..." FORCE)
endif()

include(CTest)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/loguru)

find_package(Threads)

add_executable(sample_server src/sample_server.cpp)
target_compile_options(sample_server PRIVATE -Wall)
target_link_libraries(sample_server PRIVATE Threads::Threads)
target_link_libraries(sample_server PRIVATE loguru)

install(TARGETS sample_server DESTINATION bin)

if(BUILD_TESTING)
    enable_testing()
    include(FetchContent)
	FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        release-1.8.0
	)
	
	FetchContent_GetProperties(googletest)
	if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
	endif()
endif()

