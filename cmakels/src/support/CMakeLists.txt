add_library(whereami_wrapper whereami_wrapper.cpp)
target_link_libraries(whereami_wrapper PRIVATE whereami)

add_library(support find_replace.cpp)
