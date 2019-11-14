set(CMAKELS_CMAKE_REPOSITORY "https://github.com/havogt/cmake.git")
set(CMAKELS_CMAKE_BRANCH cmakels)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    execute_process(COMMAND git clone ${CMAKELS_CMAKE_REPOSITORY} --branch ${CMAKELS_CMAKE_BRANCH} --depth 1)
endif()

execute_process(COMMAND git checkout ${CMAKELS_CMAKE_BRANCH} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
execute_process(COMMAND git pull origin ${CMAKELS_CMAKE_BRANCH} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(_BUILD_TESTING ${BUILD_TESTING})
set(BUILD_TESTING OFF)
add_subdirectory(cmake)
set(BUILD_TESTING ${_BUILD_TESTING})
unset(_BUILD_TESTING)
