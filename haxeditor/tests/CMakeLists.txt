# CMake build file for the haxeditor test program(s)

set(srcFiles
    paged_view.cpp
    hax_document_multiframe.cpp
    hax_offset_renderer.cpp
    hax_string_renderer.cpp
    selection_renderer.cpp
    selection_driver.cpp

)

add_executable(haxeditor_tests ${srcFiles})

target_link_libraries(haxeditor_tests
    haxeditor
    haxtest
    ${GTEST_MAIN_LIBRARY}
)

add_test(
    NAME haxeditor
    COMMAND $<TARGET_FILE:haxeditor_tests>
)

add_dependencies(check_targets haxeditor_tests)