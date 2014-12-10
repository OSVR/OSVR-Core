osvr_setup_lib_vars(ClientKit)

set(API
    "${HEADER_LOCATION}/ClientKit.h"
    "${HEADER_LOCATION}/ClientKitC.h"
    "${HEADER_LOCATION}/Context.h"
    "${HEADER_LOCATION}/Context_decl.h"
    "${HEADER_LOCATION}/ContextC.h"
    "${HEADER_LOCATION}/Interface.h"
    "${HEADER_LOCATION}/Interface_decl.h"
    "${HEADER_LOCATION}/InterfaceC.h"
    "${HEADER_LOCATION}/InterfaceCallbackC.h"
    "${HEADER_LOCATION}/InterfaceStateC.h"
    "${HEADER_LOCATION}/Parameters.h"
    "${HEADER_LOCATION}/ParametersC.h"
    "${HEADER_LOCATION}/SystemCallbackC.h")

set(SOURCE
    ClientKitC.cpp
    ContextC.cpp
    InterfaceC.cpp
    InterfaceCallbackC.cpp
    InterfaceStateC.cpp
    ParametersC.cpp
    SystemCallbackC.cpp)

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
    osvrUtil
    PRIVATE
    osvrClient
    osvrUtilCpp)

###
# C++ (header-only) interface
###
# Requires some version of Boost, but not C++11.
osvr_add_cpp_interface_library()
# In an installed version, we search and add the dependency in the config script
# since we don't need the same version in the same place.
target_include_directories(${LIBNAME_FULL_CPP}
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)