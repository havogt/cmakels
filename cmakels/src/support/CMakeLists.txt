add_library(whereami_wrapper whereami_wrapper.cpp)
target_link_libraries(whereami_wrapper PRIVATE whereami)

add_library(support
    find_replace.cpp
    uri_encode.cpp
)
target_compile_features(support PUBLIC cxx_std_17)
