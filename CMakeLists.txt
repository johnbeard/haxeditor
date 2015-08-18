cmake_minimum_required(VERSION 2.8)

project(haxinator)

find_package( wxWidgets 3.0.0 COMPONENTS core REQUIRED )

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

add_subdirectory(fal)
add_subdirectory(haxeditor)