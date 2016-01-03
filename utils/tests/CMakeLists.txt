# CMake build file for the haxeditor utils test program(s)

set(srcFiles
    rounding.cpp
)

add_executable(hax_utils_tests ${srcFiles})

target_link_libraries(hax_utils_tests
    hax_utils
    haxtest
    ${GTEST_MAIN_LIBRARY}
)

add_test(
    NAME hax_utils
    COMMAND $<TARGET_FILE:hax_utils_tests>
)

add_dependencies(check_targets hax_utils_tests)