# CMake build file for the haxeditor test program(s)

set(srcFiles
    mem_da.cpp
)

add_executable(dal_tests ${srcFiles})

target_link_libraries(dal_tests
    dal
    haxtest
    ${GTEST_MAIN_LIBRARY}
)

add_test(
    NAME dal
    COMMAND $<TARGET_FILE:dal_tests>
)

add_dependencies(check_targets dal_tests)