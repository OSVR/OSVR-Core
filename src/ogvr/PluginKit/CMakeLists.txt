include_directories("${HEADER_BASE}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../..")

if(BUILD_SHARED_LIBS)
    set(OGVR_PLUGINKIT_STATIC FALSE)
else()
    set(OGVR_PLUGINKIT_STATIC TRUE)
endif()
configure_file(Export.h.in "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/PluginKit")

set(API
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(SOURCE
    "${HEADER_BASE}/ogvr/Util/UniquePtr.h"
    "${HEADER_BASE}/ogvr/Util/SharedPtr.h"
    Export.h.in
    PluginRegistration.cpp
    PluginRegistrationC.cpp)

add_library(ogvrPluginKit ${API} ${SOURCE})

set_property(TARGET ogvrPluginKit PROPERTY VERSION ${ogvrpluginkit_VERSION})
set_property(TARGET ogvrPluginKit PROPERTY SOVERSION 0)
set_property(TARGET ogvrPluginKit PROPERTY INTERFACE_ogvrPluginKit_MAJOR_VERSION 0)
set_property(TARGET ogvrPluginKit APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrPluginKit_MAJOR_VERSION
)

target_include_directories(ogvrPluginKit
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../..>
    $<BUILD_INTERFACE:${HEADER_BASE}}>
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

