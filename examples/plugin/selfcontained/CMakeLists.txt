cmake_minimum_required(VERSION 2.8.12)
project(SampleExternalPlugin) # Change this line.

# This looks for an ogvrConfig.cmake file - most of the time it can be
# autodetected but you might need to specify ogvr_DIR to be something like
# C:/Users/Ryan/Desktop/build/OGVR-PluginKit-vc12/ogvr in the CMake GUI
find_package(ogvr) 

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OGVR plugin. Pass as many source files as you need.
ogvr_add_plugin(org_opengoggles_example_selfcontainedDummyAsync org_opengoggles_example_selfcontainedDummyAsync.cpp)

# This example uses the C++ wrapper, as suggested, so you'll need this line.
target_link_libraries(org_opengoggles_example_selfcontainedDummyAsync ogvr::ogvrPluginKitCpp)

# If you use other libraries, find them and add a line like:
# target_link_libraries(org_opengoggles_example_selfcontainedDummyAsync AnyOtherLibraries)

# If you access the C++ API that's not really designed for plugin consumption, you'll need this line:
# target_link_libraries(org_opengoggles_example_selfcontainedDummyAsync ogvr::ogvrPluginHost)