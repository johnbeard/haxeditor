# CMake build for the haxeditor core library.
# This library is what haxeditor implmentations (curses, wx, whatever)
# all use internally

set(srcFiles
    HaxFrame.cpp
    HaxPagedDocumentView.cpp
    HaxDocumentMultiFrame.cpp
    HaxTextCellFrame.cpp
    SelectionDriver.cpp
    DataCellRenderer.cpp
    StringCellRenderer.cpp
    
    inspection/DataInspector.cpp
    
    panels/DataInspectorPanel.cpp
)

add_library(haxeditor ${srcFiles})

target_link_libraries(haxeditor
    ${SIGC2_LIBRARIES}
    hax_utils
    dal
)

find_package(Boost REQUIRED)

# only need multiprecision for now, header only, nothing to link
target_include_directories(haxeditor
    PRIVATE SYSTEM ${Boost_INCLUDE_DIRS}
)

add_subdirectory(tests)
