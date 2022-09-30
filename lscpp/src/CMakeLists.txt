add_library(lscpp
    lsp_header.cpp
    lsp_message_handler.cpp
    lsp_launcher.cpp
    stdio_transporter.cpp
    experimental/messages.cpp
)
target_link_libraries(lscpp PRIVATE loguru)
target_include_directories(lscpp PUBLIC ${PROJECT_SOURCE_DIR}/include) #TODO install
target_compile_features(lscpp PUBLIC cxx_std_17)

add_library(lscpp_adl
    lsp_header.cpp
    stdio_transporter.cpp
    experimental/messages.cpp
)
target_link_libraries(lscpp_adl PRIVATE loguru)
target_include_directories(lscpp_adl PUBLIC ${PROJECT_SOURCE_DIR}/include) #TODO install
target_compile_features(lscpp_adl PUBLIC cxx_std_17)
