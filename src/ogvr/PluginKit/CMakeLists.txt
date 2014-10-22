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
    "${HEADER_LOCATION}/DeviceInterfaceC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${HEADER_LOCATION}/RegistrationContext.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(inc\\ogvr\\PluginKit FILES ${API})

set(UTIL_API
    "${HEADER_BASE}/ogvr/Util/UniquePtr.h"
    "${HEADER_BASE}/ogvr/Util/SharedPtr.h")
source_group(inc\\ogvr\\Util FILES ${UTIL_API})

set(SOURCE
    DeviceInterfaceC.cpp
    Export.h.in
    PluginRegistration.cpp
    PluginRegistrationC.cpp
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContext.h
    RegistrationContext.cpp)
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(ogvrPluginKit
    PUBLIC
    libfunctionality::functionality)

###
# Install library and headers
###
install(TARGETS ogvrPluginKit
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

