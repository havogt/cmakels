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

add_executable(test_setup_duplication test_setup_duplication.cpp)
target_include_directories(test_setup_duplication
    PRIVATE "${CMake_BINARY_DIR}/Source"
            "${CMake_SOURCE_DIR}/Source"
    )
target_include_directories(test_setup_duplication
    PRIVATE "${CMake_BINARY_DIR}/Utilities"
            "${CMake_SOURCE_DIR}/Utilities"
    )
target_compile_features(test_setup_duplication PUBLIC cxx_std_17)
target_link_libraries(test_setup_duplication CMakeLib)
target_link_libraries(test_setup_duplication stdc++fs)
