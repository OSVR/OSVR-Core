osvr_setup_lib_vars(Server)

set(API
    "${HEADER_LOCATION}/ConfigureServer.h"
    "${HEADER_LOCATION}/ConfigureServerFromFile.h"
    "${HEADER_LOCATION}/Server.h"
    "${HEADER_LOCATION}/ServerPtr.h"
    "${HEADER_LOCATION}/RegisterShutdownHandler.h"
    "${HEADER_LOCATION}/RegisterShutdownHandlerPOSIXSignal.h"
    "${HEADER_LOCATION}/RegisterShutdownHandlerWin32.h")

set(SOURCE
    ConfigureServer.cpp
    JSONResolvePossibleRef.h
    JSONResolvePossibleRef.cpp
    Server.cpp
    ServerImpl.cpp
    ServerImpl.h
    "${CMAKE_CURRENT_BINARY_DIR}/display_json.h")

# Fallback display descriptor
set(DISPLAY_JSON ../../../apps/displays/OSVR_HDK_1_1.json)
osvr_convert_json(display_json ${DISPLAY_JSON} "${CMAKE_CURRENT_BINARY_DIR}/display_json.h")

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrConnection
    osvrPluginHost
    PRIVATE
    osvrUtilCpp
    osvrCommon
    boost_filesystem
    vendored-vrpn
    JsonCpp::JsonCpp
    util-runloopmanager)
