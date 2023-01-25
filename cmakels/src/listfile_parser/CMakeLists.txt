add_library(listfile_parser
    listfile_parser.cpp
    listfile_query.cpp
    )

target_link_libraries(listfile_parser PUBLIC cmakels_CMakeLibWithIncludes) # TODO private once cmakels doesn't see any cmake 
target_compile_features(listfile_parser PUBLIC cxx_std_17)
