# CMake build file for the haxeditor test program(s)

set(srcFiles
    paged_view.cpp
    hax_string_renderer.cpp
    selection_renderer.cpp
)

add_executable(haxeditor_tests ${srcFiles})

target_link_libraries(haxeditor_tests
    haxeditor
    haxtest
    ${GTEST_MAIN_LIBRARY}
)

add_test(haxeditor
    haxeditor_tests
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(haxeditor_tests gcov)
endif()

target_compile_definitions(haxeditor_tests
    "-g -O0 -fprofile-arcs -ftest-coverage"
)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_target_for_coverage(haxeditor_tests_coverage haxeditor_tests coverage)
endif()

# https://git.scc.kit.edu/vj3605/ariadne_bc/blob/master/CMakeLists.txt
