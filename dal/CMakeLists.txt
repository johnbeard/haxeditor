# CMake buidfiles for DAL (data abstraction layer) library
# Part of HaxEditor
#

set(srcFiles
    DataAbstractionLayer.cpp
)

add_library(dal ${srcFiles})

add_subdirectory(tests)
