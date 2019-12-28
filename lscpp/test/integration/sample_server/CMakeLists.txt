add_test(
    integration-sample_server-direct
    ${PROJECT_BINARY_DIR}/examples/sample_server/sample_server < ${CMAKE_CURRENT_SOURCE_DIR}/comm.in.lscpp
    # WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_test(
    NAME integration-sample_server
    COMMAND
        ${CMAKE_COMMAND}
            -DWORKDIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DSAMPLE_SERVER_EXECUTABLE=${PROJECT_BINARY_DIR}/examples/sample_server/sample_server
            -P ${CMAKE_CURRENT_SOURCE_DIR}/call_sample_server.cmake
    )
