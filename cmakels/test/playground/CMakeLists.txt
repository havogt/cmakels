add_executable(test_cmake_integration test_cmake_integration.cpp)
target_include_directories(test_cmake_integration
    PRIVATE "${CMake_BINARY_DIR}/Source"
            "${CMake_SOURCE_DIR}/Source"
    )
target_include_directories(test_cmake_integration
    PRIVATE "${CMake_BINARY_DIR}/Utilities"
            "${CMake_SOURCE_DIR}/Utilities"
    )
target_link_libraries(test_cmake_integration CMakeLib)
