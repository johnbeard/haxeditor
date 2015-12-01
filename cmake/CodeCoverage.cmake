# Add a custom "coverage" target, if supported.
find_program(LCOV lcov)
if (LCOV)
    add_custom_command(
        OUTPUT coverage.info
        COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info --quiet
        COMMAND ${LCOV} --remove coverage.info '/usr/**' 'tests/*' --output-file coverage.info --quiet
        COMMENT "Generating coverage.info file"
    )

    # removes the coverage files
    add_custom_target(
        coverage_clean
        COMMAND ${CMAKE_COMMAND} -E remove coverage.info
        COMMENT "Removing coverage.info file"
    )
    find_program(GENHTML genhtml)
    if (GENHTML)
        add_custom_target(
            coverage
            COMMAND ${GENHTML} --output-directory coverage_html --quiet coverage.info --title 'HaxEditor Unit Tests'
            DEPENDS coverage.info
            DEPENDS check_targets
        )
    endif()
endif()
