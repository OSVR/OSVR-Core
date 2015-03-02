# File included only with installed binaries, to re-find Boost on those systems.

if(NOT BOOST_FOUND)
    # Optionally help Windows machines find Boost easier.
    include("${CMAKE_CURRENT_LIST_DIR}/CMakeBoostHelper.cmake" OPTIONAL)
endif()
find_package(Boost QUIET)

if(BOOST_FOUND)
    set_property(TARGET osvr::osvrUtilCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
    set_property(TARGET osvr::osvrClientKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
    set_property(TARGET osvr::osvrPluginKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()
