# CMake buidfiles for DAL (data abstraction layer) library
# Part of HaxEditor
#

find_package(Boost REQUIRED
    iostreams
    filesystem
    system
)

set(srcFiles
    DataAbstractionLayer.cpp
    ChangeStack.cpp
    ChangeApplicator.cpp
    FileDataStream.cpp
)

add_library(dal ${srcFiles})

target_include_directories(dal
	PRIVATE SYSTEM ${Boost_INCLUDE_DIRS}
)

target_link_libraries(dal
	PRIVATE ${Boost_LIBRARIES}
)

add_subdirectory(tests)
