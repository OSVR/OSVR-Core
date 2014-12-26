cmake_minimum_required(VERSION 2.8.12)
project(SampleClient) # Change this line.

# This looks for an osvrConfig.cmake file - most of the time it can be
# autodetected but you might need to specify osvr_DIR to be something like
# C:/Users/Ryan/Desktop/build/OSVR-Core-vc12/osvr in the CMake GUI
find_package(osvr)

# Pass as many source files as you need. You'll want your own since this just uses
# the regular Tracker callback demo source file (to avoid duplicate sources).
add_executable(TrackerCallbackStandalone ../TrackerCallback.cpp)

# This example uses the C++ wrapper, as suggested, so you'll need this line.
target_link_libraries(TrackerCallbackStandalone osvr::osvrClientKitCpp)

# If you use other libraries, find them and add a line like:
# target_link_libraries(TrackerCallbackStandalone AnyOtherLibraries)
