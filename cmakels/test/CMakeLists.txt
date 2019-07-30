file(GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test_*.cpp)

foreach(file IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${file} NAME_WE)

    add_executable(${test_name} ${file})
    target_link_libraries(${test_name} PRIVATE gtest_main)
    target_link_libraries(${test_name} PRIVATE cmake_query)

    # target_include_directories(${test_name}
    #     PRIVATE "${CMake_BINARY_DIR}/Source"
    #         "${CMake_SOURCE_DIR}/Source"
    # )
    # target_include_directories(${test_name}
    #     PRIVATE "${CMake_BINARY_DIR}/Utilities"
    #         "${CMake_SOURCE_DIR}/Utilities"
    # )
    # target_link_libraries(${test_name} PRIVATE CMakeLib)
    # target_compile_features(${test_name} PUBLIC cxx_std_17)
endforeach()

#add_subdirectory(playground)
