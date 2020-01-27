add_library(whereami whereami.c)
target_include_directories(whereami PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(whereami INTERFACE ${PROJECT_SOURCE_DIR}/external)
