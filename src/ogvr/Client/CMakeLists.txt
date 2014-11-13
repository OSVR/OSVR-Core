include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/Client")

set(API
    "${HEADER_LOCATION}/ClientC.h"
    "${HEADER_LOCATION}/ContextC.h"
    "${HEADER_LOCATION}/InterfaceC.h"
    "${HEADER_LOCATION}/InterfaceCallbackC.h"
    "${HEADER_LOCATION}/SystemCallbackC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    ContextC.cpp
    ContextImpl.h
    ClientC.cpp)

add_library(ogvrClient ${API} ${SOURCE})

set_target_properties(ogvrClient PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ogvrClient APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(ogvrClient
    BASE_NAME OGVR_CLIENT
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(ogvrClient PUBLIC OGVR_CLIENT_STATIC_DEFINE)
endif()

target_include_directories(ogvrClient
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${Boost_INCLUDE_DIR})

target_compile_options(ogvrClient PRIVATE ${OGVR_CXX11_FLAGS})

target_link_libraries(ogvrClient
    PRIVATE
    ogvrUtilCpp)

enable_extra_compiler_warnings(ogvrClient)

###
# Install library and headers
###
install(TARGETS ogvrClient
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/Client
    COMPONENT
    Devel)

