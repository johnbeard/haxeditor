# CMake build file for the Haxeditor shared utils library
#

set (srcFiles
	MathsUtils.cpp
)

add_library(hax_utils
	${srcFiles}
)

add_subdirectory(tests)