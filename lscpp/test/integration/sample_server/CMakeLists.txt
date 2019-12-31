# Need one file per configuration for multi-config generators (Visual Studio generator),
# otherwise the file gets written multiple times with different result which is illegal.
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/call_sample_server-$<CONFIG>.cmake INPUT call_sample_server.cmake.in)

add_test(
    NAME integration-sample_server
    COMMAND
        ${CMAKE_COMMAND}
            -DWORKDIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DSAMPLE_SERVER_EXECUTABLE=${PROJECT_BINARY_DIR}/examples/sample_server/sample_server
            -P ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/call_sample_server-$<CONFIG>.cmake
    )
