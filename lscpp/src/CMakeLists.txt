add_library(lscpp lsp_header.cpp stdio_transporter.cpp)
target_link_libraries(lscpp PUBLIC loguru) #TODO make private
target_include_directories(lscpp PUBLIC ${CMAKE_CURRENT_LIST_DIR})
