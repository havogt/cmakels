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

set(TEST_PROJECTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_cmake_query)
set(TEST_CMAKEFILES_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/config.h
    @ONLY
)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test_*.cpp)

foreach(file IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${file} NAME_WE)

    add_executable(${test_name} ${file})
    target_link_libraries(${test_name} PRIVATE gtest_main)
    target_link_libraries(${test_name} PRIVATE cmake_query)
    target_link_libraries(${test_name} PRIVATE listfile_parser)
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}) # for config (only test_cmake_query)
    target_compile_features(${test_name} PUBLIC cxx_std_17)
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src)

    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach()

if(BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()

add_subdirectory(debugging)

add_executable(debug_cmake_query debug_cmake_query.cpp)
target_link_libraries(debug_cmake_query PRIVATE cmake_query)
# target_link_libraries(debug_cmake_query PRIVATE listfile_parser)
target_include_directories(debug_cmake_query PRIVATE ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}) # for config (only test_cmake_query)
target_compile_features(debug_cmake_query PUBLIC cxx_std_17)
target_include_directories(debug_cmake_query PRIVATE ${PROJECT_SOURCE_DIR}/src)
