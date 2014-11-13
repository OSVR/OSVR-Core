ogvr_setup_lib_vars(Connection)

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
    VrpnMessageType.h)

ogvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OGVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    ogvrPluginHost
    ogvrUtilCpp
    vendored-vrpn
    util-runloopmanager
    qvrpn-headers)
