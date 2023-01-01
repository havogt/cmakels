cmake_minimum_required(VERSION 3.12.4)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(lscpp LANGUAGES CXX)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../install" CACHE PATH "..." FORCE)
endif()

include(CTest)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/loguru)

find_package(Threads)

add_subdirectory(src)

# add_executable(sample_server src/sample_server.cpp)
# target_compile_options(sample_server PRIVATE -Wall)
# target_link_libraries(sample_server PRIVATE Threads::Threads)
# target_link_libraries(sample_server PRIVATE loguru)
# target_link_libraries(sample_server PRIVATE lscpp)
# install(TARGETS sample_server DESTINATION bin)

add_subdirectory(examples)

if(BUILD_TESTING)
	add_subdirectory(test)
endif()
