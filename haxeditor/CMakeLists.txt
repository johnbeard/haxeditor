
set(srcFiles
    HaxDocumentMultiFrame.cpp
)

add_library(haxeditor ${srcFiles})

target_link_libraries(haxeditor
    fal
)