# CMake external project support for GTest and GMock

# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Single download(git clone)
ExternalProject_Add(
    googletest
    DOWNLOAD_DIR "googletest-master/src" # The only dir option which is required
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG "release-1.7.0"
    TIMEOUT 10
    LOG_DOWNLOAD ON
    # Disable all other steps
    INSTALL_COMMAND ""
)

externalproject_get_property(googletest source_dir)
externalproject_get_property(googletest binary_dir)

set(GTEST_INCLUDE_DIRS ${source_dir}/include)
set(GTEST_LIBRARY_PATH ${binary_dir})

set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_property(
    TARGET ${GTEST_LIBRARY}
    PROPERTY IMPORTED_LOCATION
    ${GTEST_LIBRARY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(${GTEST_LIBRARY} googletest)

set(GTEST_MAIN_LIBRARY gtest-main)
add_library(${GTEST_MAIN_LIBRARY} UNKNOWN IMPORTED)

set_property(
    TARGET ${GTEST_MAIN_LIBRARY}
    PROPERTY IMPORTED_LOCATION
    ${GTEST_LIBRARY_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
)
add_dependencies(${GTEST_MAIN_LIBRARY} googletest)
