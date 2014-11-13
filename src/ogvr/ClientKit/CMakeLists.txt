include_directories("${HEADER_BASE}")
include_directories("${BUILDTREE_HEADER_BASE}")

ogvr_setup_lib_vars(ClientKit)

set(API
    "${HEADER_LOCATION}/ClientKitC.h"
    "${HEADER_LOCATION}/ContextC.h"
    "${HEADER_LOCATION}/InterfaceC.h"
    "${HEADER_LOCATION}/InterfaceCallbackC.h"
    "${HEADER_LOCATION}/SystemCallbackC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
source_group(API FILES ${API})

set(SOURCE
    ClientKitC.cpp
    ContextC.cpp
    InterfaceC.cpp
    InterfaceCallbackC.cpp
    SystemCallbackC.cpp)

add_library(${LIBNAME_FULL} ${API} ${SOURCE})

set_target_properties(${LIBNAME_FULL} PROPERTIES
    FOLDER "${LIB_FOLDER}"
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION
)

generate_export_header(${LIBNAME_FULL}
    BASE_NAME ${EXPORT_BASENAME}
    EXPORT_FILE_NAME Export.h)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${LIBNAME_FULL} PUBLIC ${EXPORT_BASENAME}_STATIC_DEFINE)
endif()

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PRIVATE
    ${OGVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    ogvrClient
    ogvrUtilCpp)

enable_extra_compiler_warnings(${LIBNAME_FULL})

###
# Install library and headers
###
install(TARGETS ${LIBNAME_FULL}
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/${LIBNAME}
    COMPONENT
    Devel)
