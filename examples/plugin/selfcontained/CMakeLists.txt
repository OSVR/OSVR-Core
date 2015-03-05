cmake_minimum_required(VERSION 2.8.12)
project(SampleExternalPlugin) # Change this line.

# This looks for an osvrConfig.cmake file - most of the time it can be
# autodetected but you might need to create/extend CMAKE_PREFIX_PATH to include something like
# C:/Users/Ryan/Desktop/build/OSVR-Core-vc12 or
# C:/Users/Ryan/Downloads/OSVR-Core-Snapshot-v0.1-406-gaa55515-build54-vs12-32bit
# in the CMake GUI or command line.
find_package(osvr)

# This generates a header file, from the named json file, containing a string literal
# named com_osvr_example_selfcontainedDetectAndCreate_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_example_selfcontainedDetectAndCreate_json
    com_osvr_example_selfcontainedDetectAndCreate.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontainedDetectAndCreate_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OSVR plugin. It also installs the plugin into the right directory.
# Pass as many source files as you need.
osvr_add_plugin(com_osvr_example_selfcontainedDetectAndCreate
    com_osvr_example_selfcontainedDetectAndCreate.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontainedDetectAndCreate_json.h")

# This example uses the C++ wrapper, as suggested, so you'll need this line.
target_link_libraries(com_osvr_example_selfcontainedDetectAndCreate osvr::osvrPluginKitCpp)

# If you use other libraries, find them and add a line like:
# target_link_libraries(com_osvr_example_selfcontainedDetectAndCreate AnyOtherLibraries)

# If you access the C++ API that's not really designed for plugin consumption, you'll need this line:
# target_link_libraries(com_osvr_example_selfcontainedDetectAndCreate osvr::osvrPluginHost)