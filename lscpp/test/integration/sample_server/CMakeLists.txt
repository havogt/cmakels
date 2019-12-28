# add_test(
#     integration-simple_comm
#     ${PROJECT_BINARY_DIR}/examples/sample_server/sample_server < ${CMAKE_CURRENT_SOURCE_DIR}/comm.in
#     # WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     )

add_test(
    NAME integration-simple_comm
    COMMAND
        ${CMAKE_COMMAND}
            -DWORKDIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DSAMPLE_SERVER_EXECUTABLE=${PROJECT_BINARY_DIR}/examples/sample_server/sample_server
            -P ${CMAKE_CURRENT_SOURCE_DIR}/call_sample_server.cmake
    )
