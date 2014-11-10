include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/Connection")

set(API
    "${HEADER_LOCATION}/Connection.h"
    "${HEADER_LOCATION}/ConnectionDevicePtr.h"
    "${HEADER_LOCATION}/ConnectionPtr.h"
    "${HEADER_LOCATION}/DeviceToken.h"
    "${HEADER_LOCATION}/MessageType.h"
    "${HEADER_LOCATION}/MessageTypePtr.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    AsyncAccessControl.cpp
    AsyncAccessControl.h
    AsyncDeviceToken.cpp
    AsyncDeviceToken.h
    Connection.cpp
    ConnectionDevice.cpp
    ConnectionDevice.h
    DeviceToken.cpp
    MessageType.cpp
    SyncDeviceToken.cpp
    SyncDeviceToken.h
    VrpnBaseFlexServer.h
    VrpnBasedConnection.cpp
    VrpnBasedConnection.h
    VrpnConnectionDevice.h
    VrpnMessageType.h)

add_library(ogvrConnection ${API} ${SOURCE})

set_target_properties(ogvrConnection PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ogvrConnection APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(ogvrConnection
    BASE_NAME OGVR_CONNECTION
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(ogvrConnection PUBLIC OGVR_CONNECTION_STATIC_DEFINE)
endif()

target_include_directories(ogvrConnection
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIR}
    PRIVATE
    ${VRPN_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/vendor/qvrpn
    ${PROJECT_SOURCE_DIR}/vendor/util-headers)

target_compile_options(ogvrConnection PUBLIC ${OGVR_CXX11_FLAGS})

if(NOT Boost_USE_STATIC_LIBS)
    target_compile_definitions(ogvrConnection PRIVATE BOOST_ALL_DYN_LINK)
endif()

target_link_libraries(ogvrConnection
    PRIVATE
    ogvrPluginHost
    ogvrUtilCpp
    vendored-vrpn
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

enable_extra_compiler_warnings(ogvrConnection)

###
# Install library and headers
###
install(TARGETS ogvrConnection
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/Connection
    COMPONENT
    Devel)

