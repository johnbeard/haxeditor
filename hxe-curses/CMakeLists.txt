
set(srcFiles
    hxe_curses.cpp
)

find_package(Curses REQUIRED)

include_directories(
    ${CURSES_INCLUDE_DIR}
)

add_executable(hxe-curses ${srcFiles})

target_link_libraries(hxe-curses
    haxeditor
    ${CURSES_LIBRARIES}
)