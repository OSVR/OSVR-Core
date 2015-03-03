# File included only with installed binaries, to re-find Boost on those systems.

if(NOT BOOST_FOUND)
    # Optionally help Windows machines find Boost easier.
    include("${CMAKE_CURRENT_LIST_DIR}/CMakeBoostHelper.cmake" OPTIONAL)
endif()
find_package(Boost QUIET)

if(BOOST_FOUND)
    # Boost is optional for UtilCpp - lots of stuff needs it, but lots doesn't.
    set_property(TARGET osvr::osvrUtilCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()

# Boost is required for these bindings. Adding regardless of if we found it - make the build fail early if it's not found.
set_property(TARGET osvr::osvrClientKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET osvr::osvrPluginKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
