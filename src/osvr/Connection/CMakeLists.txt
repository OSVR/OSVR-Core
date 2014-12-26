osvr_setup_lib_vars(Connection)

set(API
    "${HEADER_LOCATION}/Connection.h"
    "${HEADER_LOCATION}/ConnectionDevicePtr.h"
    "${HEADER_LOCATION}/ConnectionDevice.h"
    "${HEADER_LOCATION}/ConnectionPtr.h"
    "${HEADER_LOCATION}/DeviceToken.h"
    "${HEADER_LOCATION}/MessageType.h"
    "${HEADER_LOCATION}/MessageTypePtr.h")

set(SOURCE
    AsyncAccessControl.cpp
    AsyncAccessControl.h
    AsyncDeviceToken.cpp
    AsyncDeviceToken.h
    Connection.cpp
    ConnectionDevice.cpp
    DeviceToken.cpp
    GenericConnectionDevice.h
    MessageType.cpp
    SyncDeviceToken.cpp
    SyncDeviceToken.h
    VrpnBaseFlexServer.h
    VrpnBasedConnection.cpp
    VrpnBasedConnection.h
    VrpnConnectionDevice.h
    VrpnConnectionKind.cpp
    VrpnConnectionKind.h
    VrpnMessageType.h)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

if(WIN32 AND NOT MSVC AND Boost_USE_STATIC_LIBS)
    # Work around a foolish insistence to use DLLs on MXE that don't exist.
    target_compile_definitions(${LIBNAME_FULL}
        PRIVATE
        BOOST_THREAD_USE_LIB)
endif()

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    osvrPluginHost
    osvrUtilCpp
    vendored-vrpn
    util-runloopmanager)
