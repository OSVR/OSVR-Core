include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/Server")

set(API
    "${HEADER_LOCATION}/Server.h"
    "${HEADER_LOCATION}/ServerPtr.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    Server.cpp
    ServerImpl.cpp
    ServerImpl.h)

add_library(ogvrServer ${API} ${SOURCE})

set_target_properties(ogvrServer PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ogvrServer APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(ogvrServer
    BASE_NAME OGVR_SERVER
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(ogvrServer PUBLIC OGVR_SERVER_STATIC_DEFINE)
endif()

target_include_directories(ogvrServer
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIR})

target_compile_options(ogvrServer PRIVATE ${OGVR_CXX11_FLAGS})

if(NOT Boost_USE_STATIC_LIBS)
    target_compile_definitions(ogvrServer PRIVATE BOOST_ALL_DYN_LINK)
endif()

target_link_libraries(ogvrServer
    PRIVATE
    ogvrConnection
    ogvrPluginHost
    ogvrUtilCpp
    util-runloopmanager)

enable_extra_compiler_warnings(ogvrServer)

###
# Install library and headers
###
install(TARGETS ogvrServer
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/Server
    COMPONENT
    Devel)

