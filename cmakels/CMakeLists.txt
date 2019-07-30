cmake_minimum_required(VERSION 3.14.5)

project(cmakels LANGUAGES C CXX)

add_subdirectory(../lscpp ${CMAKE_CURRENT_BINARY_DIR}/lscpp)
add_subdirectory(external/cmake)
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
	FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG        release-1.8.1
	)
    FetchContent_MakeAvailable(googletest)

	add_subdirectory(test)
endif()
