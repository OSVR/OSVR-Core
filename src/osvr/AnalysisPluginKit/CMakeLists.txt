osvr_setup_lib_vars(AnalysisPluginKit)

set(API
    "${HEADER_LOCATION}/AnalysisPluginKitC.h")

set(SOURCE
    AnalysisPluginKitC.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    INTERFACE
    osvrPluginKit
    osvrClientKit
    PRIVATE
    osvrPluginKit
    osvrPluginHost
    osvrConnection
    osvrUtilCpp
    osvrCommon
    osvrClient
    vendored-vrpn)

# Add library alias to allow use of the same osvr_add_plugin script within this
# tree and outside of it.
add_library(osvr::${LIBNAME_FULL} ALIAS ${LIBNAME_FULL})

###
# C++ (header-only) interface
###
# Requires some version of Boost, and util for time, but not C++11.
#osvr_add_cpp_interface_library()
#target_link_libraries(${LIBNAME_INTERFACE}
#    INTERFACE
#    osvrUtil)

# In an installed version, we search and add the dependency in the
# osvrConfigInstalledBoost.cmake script since we don't need the same version in
# the same place. Thus, here we only add it to the interface if using a build tree.
#target_include_directories(${LIBNAME_INTERFACE}
#    INTERFACE
#    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

# Add library alias to allow use of the same osvr_add_plugin script within this
# tree and outside of it.
#add_library(osvr::${LIBNAME_INTERFACE} ALIAS ${LIBNAME_INTERFACE})
