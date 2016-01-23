# CMake build file for the haxeditor test program(s)

set(srcFiles
    mem_da.cpp
    change_stack.cpp
    change_applicator.cpp
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

set(sampleFileDir $<TARGET_FILE_DIR:dal_tests>/sample_files)

#message(FATAL_ERROR ${sampleFileDir})

# copy the sample files to the build dir
add_custom_command(TARGET dal_tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/sample_files ${sampleFileDir}
	COMMENT "Copying DAL sample_files to ${sampleFileDir}"
)

target_compile_definitions(dal_tests PRIVATE
	-DDALTEST_SAMPLE_FILES="${sampleFileDir}"
)

add_dependencies(check_targets dal_tests)
