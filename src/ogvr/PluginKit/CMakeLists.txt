include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

if(BUILD_SHARED_LIBS)
    set(OGVR_PLUGINKIT_STATIC FALSE)
else()
    set(OGVR_PLUGINKIT_STATIC TRUE)
endif()
configure_file(Export.h.in "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/PluginKit")

set(API
    "${HEADER_LOCATION}/AsyncDeviceToken.h"
    "${HEADER_LOCATION}/CommonC.h"
    "${HEADER_LOCATION}/Connection.h"
    "${HEADER_LOCATION}/ConnectionPtr.h"
    "${HEADER_LOCATION}/ConnectionDevice.h"
    "${HEADER_LOCATION}/ConnectionDevicePtr.h"
    "${HEADER_LOCATION}/CreatePluginSpecificRegistrationContext.h"
    "${HEADER_LOCATION}/DeviceInterface.h"
    "${HEADER_LOCATION}/DeviceInterfaceC.h"
    "${HEADER_LOCATION}/DeviceToken.h"
    "${HEADER_LOCATION}/MessageType.h"
    "${HEADER_LOCATION}/MessageTypePtr.h"
    "${HEADER_LOCATION}/PluginInterfaceC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${HEADER_LOCATION}/RegistrationContext.h"
    "${HEADER_LOCATION}/SyncDeviceToken.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(inc\\ogvr\\PluginKit FILES ${API})

set(UTIL_API
    "${HEADER_BASE}/ogvr/Util/CallbackWrapper.h"
    "${HEADER_BASE}/ogvr/Util/GenericDeleter.h"
    "${HEADER_BASE}/ogvr/Util/ResourcePath.h"
    "${HEADER_BASE}/ogvr/Util/UniquePtr.h"
    "${HEADER_BASE}/ogvr/Util/TimeValue.h"
    "${HEADER_BASE}/ogvr/Util/TimeValueC.h"
    "${HEADER_BASE}/ogvr/Util/SharedPtr.h")
source_group(inc\\ogvr\\Util FILES ${UTIL_API})

set(SOURCE
    AsyncDeviceToken.cpp
    Connection.cpp
    ConnectionDevice.cpp
    CreatePluginSpecificRegistrationContext.cpp
    DeviceInterfaceC.cpp
    DeviceToken.cpp
    Export.h.in
    HandleNullContext.h
    MessageType.cpp
    PluginRegistration.cpp
    PluginRegistrationC.cpp
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContext.h
    RegistrationContext.cpp
    ResetPointerList.h
    SyncDeviceToken.cpp
    VrpnBasedConnection.cpp
    VrpnBasedConnection.h
    VrpnConnectionDevice.h
    VrpnBaseFlexServer.h
    VrpnMessageType.h)
source_group(src\\ogvr\\PluginKit FILES ${SOURCE})
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
add_library(ogvrPluginKit ${API} ${UTIL_API} ${SOURCE})

set_property(TARGET ogvrPluginKit PROPERTY VERSION ${ogvrpluginkit_VERSION})
set_property(TARGET ogvrPluginKit PROPERTY SOVERSION 0)
set_property(TARGET ogvrPluginKit PROPERTY INTERFACE_ogvrPluginKit_MAJOR_VERSION 0)
set_property(TARGET ogvrPluginKit APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrPluginKit_MAJOR_VERSION
)

target_include_directories(ogvrPluginKit
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${VRPN_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/vendor/qvrpn
    ${Boost_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/vendor/util-headers)

target_compile_options(ogvrPluginKit PRIVATE ${OGVR_CXX11_FLAGS})

target_compile_definitions(ogvrPluginKit PRIVATE ${VRPN_DEFINITIONS} BOOST_DYN_LINK_ALL)

target_link_libraries(ogvrPluginKit
    INTERFACE
    libfunctionality::functionality-plugininterface
    PRIVATE
    ogvrUtil
    libfunctionality::functionality
    ${VRPN_SERVER_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_CHRONO_LIBRARY})

###
# Internal "interface" target for host applications to use - they need more includes
# and C++11 support.
###
add_library(ogvrPluginHost INTERFACE)
target_include_directories(ogvrPluginHost
    INTERFACE
    ${Boost_INCLUDE_DIR})
target_compile_options(ogvrPluginHost INTERFACE ${OGVR_CXX11_FLAGS})
target_link_libraries(ogvrPluginHost INTERFACE ogvrPluginKit
    ogvrUtil
    libfunctionality::functionality)

###
# Grab DLLs please.
###
if(WIN32)
    add_custom_command(TARGET ogvrPluginKit POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libfunctionality::functionality> $<TARGET_FILE_DIR:ogvrPluginKit>
        COMMENT Copying required DLL
        VERBATIM)
endif()

###
# Install library and headers
###
install(TARGETS ogvrPluginKit ogvrPluginHost
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/PluginKit
    COMPONENT
    Devel)

