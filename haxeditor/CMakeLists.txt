
set(srcFiles
    HaxDocumentMultiFrame.cpp
)

add_library(haxeditor ${srcFiles})

target_link_libraries(haxeditor
    ${SIGC2_LIBRARIES}
    fal
)
