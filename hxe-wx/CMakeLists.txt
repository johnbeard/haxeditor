set(srcFiles
    hxe-wx.cpp
    HexTextFrame.cpp
    HexMultiFrame.cpp
    HugeScrollBar.cpp
    
    panels/HwxDataInspectorPanel.cpp
)

add_executable(hxe-wx ${srcFiles})

target_link_libraries(hxe-wx
	haxeditor
	${wxWidgets_LIBRARIES}
)
