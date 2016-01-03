# CMake build for the haxeditor core library.
# This library is what haxeditor implmentations (curses, wx, whatever)
# all use internally

set(srcFiles
    HaxFrame.cpp
    HaxDocumentMultiFrame.cpp
)

add_library(haxeditor ${srcFiles})

target_link_libraries(haxeditor
    ${SIGC2_LIBRARIES}
    dal
)

add_subdirectory(tests)
