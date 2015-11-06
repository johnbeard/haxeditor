# CMake build file for the haxeditor test program(s)

set(srcFiles
    paged_view.cpp
)

add_executable(haxeditor_tests ${srcFiles})

target_link_libraries(haxeditor_tests
    haxtest
    ${GTEST_MAIN_LIBRARY}
)