cmake_minimum_required(VERSION 3.12.4)

execute_process(
    COMMAND ${SAMPLE_SERVER_EXECUTABLE}
    OUTPUT_VARIABLE outvar
    RESULT_VARIABLE resvar
    INPUT_FILE comm.in
    )

if(NOT resvar EQUAL 0)
    message(FATAL_ERROR "Non zero exit code received from ${SAMPLE_SERVER_EXECUTABLE}!")
endif()

set(expected_response_to_shutdown "{\"id\":1,\"jsonrpc\":\"2.0\",\"result\":null}")
if(NOT outvar MATCHES ${expected_response_to_shutdown})
    message(FATAL_ERROR "Did not receive proper response to shutdown request!")
endif()

message(STATUS ${outvar})
