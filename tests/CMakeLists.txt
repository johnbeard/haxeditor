
add_library(haxtest
    haxtest.cpp
)

target_include_directories(haxtest
    PUBLIC ${GTEST_INCLUDE_DIRS}
)

# enable code coverage on tests
target_compile_options(haxtest
    PUBLIC -g -O0 -fprofile-arcs -ftest-coverage
)

target_link_libraries(haxtest
    gtest
    gcov
)
