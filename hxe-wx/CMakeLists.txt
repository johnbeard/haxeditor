set(srcFiles
    hxe-wx.cpp
    hexframe.cpp
)

add_executable(hxe-wx ${srcFiles})

target_link_libraries(hxe-wx ${wxWidgets_LIBRARIES})