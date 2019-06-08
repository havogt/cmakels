add_library(lscpp lsp_header.cpp stdio_transporter.cpp)
target_link_libraries(lscpp PRIVATE loguru)
target_include_directories(lscpp PUBLIC ${CMAKE_CURRENT_LIST_DIR})
