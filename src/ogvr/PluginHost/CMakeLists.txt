include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/PluginHost")

set(API
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext_fwd.h"
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext.h"
    "${HEADER_LOCATION}/PluginRegPtr.h"
    "${HEADER_LOCATION}/RegistrationContext_fwd.h"
    "${HEADER_LOCATION}/RegistrationContext.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContextImpl.cpp
    PluginSpecificRegistrationContextImpl.h
    RegistrationContext.cpp)

add_library(ogvrPluginHost ${API} ${SOURCE})

set_target_properties(ogvrPluginHost PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)
set_property(TARGET ogvrPluginHost APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(ogvrPluginHost
    BASE_NAME OGVR_PLUGINHOST
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(ogvrPluginHost PUBLIC OGVR_PLUGINHOST_STATIC_DEFINE)
endif()

target_include_directories(ogvrPluginHost
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIR})

target_compile_options(ogvrPluginHost PUBLIC ${OGVR_CXX11_FLAGS})

target_link_libraries(ogvrPluginHost
    PUBLIC
    libfunctionality::functionality
    ogvrUtilCpp)

enable_extra_compiler_warnings(ogvrPluginHost)

###
# Grab DLLs please.
###
if(WIN32)
    add_custom_command(TARGET ogvrPluginHost POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libfunctionality::functionality> $<TARGET_FILE_DIR:ogvrPluginHost>
        COMMENT Copying required DLL
        VERBATIM)
endif()

###
# Install library and headers
###
install(TARGETS ogvrPluginHost
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/PluginHost
    COMPONENT
    Devel)

