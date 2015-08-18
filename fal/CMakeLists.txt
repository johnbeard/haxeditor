
set(srcFiles
    FAL.cpp
)

add_library(fal ${srcFiles})

target_link_libraries(fal ${wxWidgets_LIBRARIES})