set(srcFiles
    hxe-wx.cpp
    hexframe.cpp
    HexMultiFrame.cpp
    HugeScrollBar.cpp
)

add_executable(hxe-wx ${srcFiles})

target_link_libraries(hxe-wx
	haxeditor
	${wxWidgets_LIBRARIES}
)