add_test(
    NAME integration-simple_comm
    COMMAND ${CMAKE_COMMAND} -DSAMPLE_SERVER_EXECUTABLE=${PROJECT_BINARY_DIR}/examples/sample_server/sample_server -P ${CMAKE_CURRENT_LIST_DIR}/call_sample_server.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
