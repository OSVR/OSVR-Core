include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/VRPN")

set(API
    "${HEADER_LOCATION}/ConstructVRPNObject.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    ConstructVRPNObject.cpp)

add_library(ogvrVRPN ${API} ${SOURCE})

set_target_properties(ogvrVRPN PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ogvrVRPN APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(ogvrVRPN
    BASE_NAME OGVR_VRPN
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(ogvrVRPN PUBLIC OGVR_VRPN_STATIC_DEFINE)
endif()

target_include_directories(ogvrVRPN
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIR}
    PRIVATE
    ${VRPN_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/vendor/qvrpn
    ${PROJECT_SOURCE_DIR}/vendor/util-headers)

target_compile_options(ogvrVRPN PRIVATE ${OGVR_CXX11_FLAGS})


target_link_libraries(ogvrVRPN
    PRIVATE
    ogvrUtilCpp
    vendored-vrpn)

enable_extra_compiler_warnings(ogvrVRPN)

###
# Install library and headers
###
install(TARGETS ogvrVRPN
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/VRPN
    COMPONENT
    Devel)
