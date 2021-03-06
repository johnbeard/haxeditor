cmake_minimum_required(VERSION 2.8.12)

if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
    set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
    set(CMAKE_ECLIPSE_VERSION "4.4" CACHE STRING "Eclipse version" FORCE)
endif()

project(haxeditor)

# custom package finders
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package( wxWidgets 2.8.0 COMPONENTS core REQUIRED )

find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGC2 REQUIRED sigc++-2.0)
include_directories(${SIGC2_INCLUDE_DIRS})

# add testing support
include(GoogleTestProjects)
enable_testing()

# target to gather test dependencies
add_custom_target(check_targets)

# target to build tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS check_targets
)

# code coverage for tests
include(CodeCoverage)

add_definitions(
    -Wall -Wextra -Wpedantic
)

set(CMAKE_CXX_STANDARD 11)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
add_subdirectory(utils)
add_subdirectory(fal)
add_subdirectory(dal)
add_subdirectory(haxeditor)
#add_subdirectory(hxe-curses)
add_subdirectory(hxe-wx)

