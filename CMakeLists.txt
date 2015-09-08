cmake_minimum_required(VERSION 2.8)

if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
    set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
    set(CMAKE_ECLIPSE_VERSION "4.4" CACHE STRING "Eclipse version" FORCE)
endif() 

project(haxeditor)

find_package( wxWidgets 3.0.0 COMPONENTS core REQUIRED )

add_definitions(
    -Wall -Wextra -Wpedantic
)

set(CMAKE_CXX_STANDARD 11)



# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(fal)
add_subdirectory(haxeditor)
#add_subdirectory(hxe-curses)
add_subdirectory(hxe-wx)