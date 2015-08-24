osvr_setup_lib_vars(JointClientKit)

set(API
    "${HEADER_LOCATION}/JointClientKitC.h")

set(SOURCE
    JointClientKitC.cpp)

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
    PUBLIC
    osvrClientKit
    osvrUtil
    PRIVATE
    osvrClient
    osvrUtilCpp
    osvrCommon
    eigen-headers)

###
# C++ (header-only) interface
###
# Requires some version of Boost, but not C++11.
#osvr_add_cpp_interface_library()
# In an installed version, we search and add the dependency in the config script
# since we don't need the same version in the same place.
#target_include_directories(${LIBNAME_INTERFACE}
#    INTERFACE
#    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
