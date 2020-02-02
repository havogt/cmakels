include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()


file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test_*.cpp)

foreach(file IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${file} NAME_WE)

    add_executable(${test_name} ${file})
    if(NOT ${file} MATCHES ".*nogtest.*")
        target_link_libraries(${test_name} PRIVATE gtest_main)
    endif()
    target_link_libraries(${test_name} PRIVATE cmake_query)
    target_compile_features(${test_name} PUBLIC cxx_std_17)

    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach()

if(BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()

add_subdirectory(debugging)
