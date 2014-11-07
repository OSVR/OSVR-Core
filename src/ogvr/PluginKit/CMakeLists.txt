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
    "${HEADER_LOCATION}/PluginKit.h"
    "${HEADER_LOCATION}/PluginKitC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    DeviceInterfaceC.cpp
    Export.h.in
    HandleNullContext.h
    PluginRegistrationC.cpp)

add_library(ogvrPluginKit ${API} ${SOURCE})
set_target_properties(ogvrPluginKit PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)
set_property(TARGET ogvrPluginKit APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

target_include_directories(ogvrPluginKit
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_compile_options(ogvrPluginKit PRIVATE ${OGVR_CXX11_FLAGS})

target_link_libraries(ogvrPluginKit
    INTERFACE
    libfunctionality::functionality-plugininterface
    PRIVATE
    ogvrPluginHost
    ogvrConnection
    ogvrUtilCpp)

enable_extra_compiler_warnings(ogvrPluginKit)

###
# C++ (header-only) interface
###
# Requires some version of Boost, but not C++11.
add_library(ogvrPluginKitCpp INTERFACE)
target_link_libraries(ogvrPluginKitCpp INTERFACE ogvrPluginKit)
target_include_directories(ogvrPluginKitCpp
    INTERFACE ${Boost_INCLUDE_DIRS})

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
