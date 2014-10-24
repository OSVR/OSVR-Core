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
    "${HEADER_LOCATION}/CommonC.h"
    "${HEADER_LOCATION}/DeviceInterface.h"
    "${HEADER_LOCATION}/DeviceInterfaceC.h"
    "${HEADER_LOCATION}/PluginInterfaceC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${HEADER_LOCATION}/RegistrationContext.h"
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
    DeviceInterfaceC.cpp
    Export.h.in
    PluginRegistration.cpp
    PluginRegistrationC.cpp
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContext.h
    RegistrationContext.cpp
    ResetPointerList.h)
source_group(src\\ogvr\\PluginKit FILES ${SOURCE})

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
    $<INSTALL_INTERFACE:include>)

target_include_directories(ogvrPluginKit
    PRIVATE
    ${Boost_INCLUDE_DIR})

target_link_libraries(ogvrPluginKit
    PUBLIC
    ogvrUtil
    libfunctionality::functionality)

###
# Internal "interface" target for host applications to use - they need more includes.
###
add_library(ogvrPluginKit-hostinterface INTERFACE)
target_include_directories(ogvrPluginKit-hostinterface
    INTERFACE
    ${Boost_INCLUDE_DIR})
target_link_libraries(ogvrPluginKit-hostinterface INTERFACE ogvrPluginKit)

###
# Install library and headers
###
install(TARGETS ogvrPluginKit ogvrPluginKit-hostinterface
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

