add_library(lscpp
    lsp_header.cpp
    lsp_message_handler.cpp
    lsp_launcher.cpp
    stdio_transporter.cpp
    )
target_link_libraries(lscpp PRIVATE loguru)
target_include_directories(lscpp PUBLIC ${PROJECT_SOURCE_DIR}/include) #TODO install
target_compile_features(lscpp PUBLIC cxx_std_17)
