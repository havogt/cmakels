function(add_lscpp_test)
    set(options)
    set(oneValueArgs SOURCE)
    set(multiValueArgs LIBRARIES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(test_name ${ARG_SOURCE} NAME_WE)
    add_executable(${test_name} ${ARG_SOURCE})
    target_link_libraries(${test_name} gtest_main ${ARG_LIBRARIES})
    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endfunction()

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

add_lscpp_test(SOURCE test_read_header.cpp LIBRARIES lscpp)
# add_lscpp_test(SOURCE test_message_handling_v1.cpp)
add_lscpp_test(SOURCE test_message_handling_v2.cpp)
add_lscpp_test(SOURCE test_json_serializable.cpp)
add_lscpp_test(SOURCE test_std_variant_visitor.cpp)

add_subdirectory(integration)
