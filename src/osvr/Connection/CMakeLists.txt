osvr_setup_lib_vars(Connection)

set(API
    "${HEADER_LOCATION}/AnalogServerInterface.h"
    "${HEADER_LOCATION}/ButtonServerInterface.h"
    "${HEADER_LOCATION}/Connection.h"
    "${HEADER_LOCATION}/ConnectionDevicePtr.h"
    "${HEADER_LOCATION}/ConnectionDevice.h"
    "${HEADER_LOCATION}/ConnectionPtr.h"
    "${HEADER_LOCATION}/DeviceInitObject.h"
    "${HEADER_LOCATION}/DeviceToken.h"
    "${HEADER_LOCATION}/MessageType.h"
    "${HEADER_LOCATION}/MessageTypePtr.h"
    "${HEADER_LOCATION}/TrackerServerInterface.h")

set(SOURCE
    AsyncAccessControl.cpp
    AsyncAccessControl.h
    AsyncDeviceToken.cpp
    AsyncDeviceToken.h
    Connection.cpp
    ConnectionDevice.cpp
    DeviceConstructionData.h
    DeviceInitObject.cpp
    DeviceToken.cpp
    GenerateCompoundServer.h
    GenerateVrpnDynamicServer.cpp
    GenerateVrpnDynamicServer.h
    GenericConnectionDevice.h
    MessageType.cpp
    SyncDeviceToken.cpp
    SyncDeviceToken.h
    VirtualDeviceToken.cpp
    VirtualDeviceToken.h
    VrpnAnalogServer.h
    VrpnBaseFlexServer.h
    VrpnBasedConnection.cpp
    VrpnBasedConnection.h
    VrpnButtonServer.h
    VrpnConnectionDevice.h
    VrpnConnectionKind.cpp
    VrpnConnectionKind.h
    VrpnMessageType.h
    VrpnTrackerServer.h)

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

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrPluginHost
    INTERFACE
    osvrUtil
    PRIVATE
    osvrUtilCpp
    vendored-vrpn
    util-runloopmanager)
