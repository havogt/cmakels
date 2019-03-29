add_library(lscpp lsp_header.cpp stdio_transporter.cpp)
target_link_libraries(lscpp PRIVATE loguru)
