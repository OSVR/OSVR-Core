###
# Internal Verbosity Header
###
if(BUILD_DEV_VERBOSE)
    set(OGVR_UTIL_DEV_VERBOSE TRUE)
endif()
configure_file(Verbosity.h.in "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

###
# Platform Config Header
###
include(CheckStructHasMember)
check_struct_has_member("struct timeval" tv_sec sys/time.h OGVR_HAVE_STRUCT_TIMEVAL_IN_SYS_TIME_H LANGUAGE C)
check_struct_has_member("struct timeval" tv_sec winsock2.h OGVR_HAVE_STRUCT_TIMEVAL_IN_WINSOCK2_H LANGUAGE C)
configure_file(PlatformConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")

###
# Library build
###
set(HEADER_LOCATION "${HEADER_BASE}/ogvr/Util")

set(API
    "${HEADER_LOCATION}/APIBaseC.h"
    "${HEADER_LOCATION}/AnnotationMacrosC.h"
    "${HEADER_LOCATION}/AnyMap.h"
    "${HEADER_LOCATION}/CallbackWrapper.h"
    "${HEADER_LOCATION}/DeviceCallbackTypesC.h"
    "${HEADER_LOCATION}/GenericCaller.h"
    "${HEADER_LOCATION}/GenericDeleter.h"
    "${HEADER_LOCATION}/MacroToolsC.h"
    "${HEADER_LOCATION}/PluginCallbackTypesC.h"
    "${HEADER_LOCATION}/PluginRegContextC.h"
    "${HEADER_LOCATION}/ResourcePath.h"
    "${HEADER_LOCATION}/ReturnCodesC.h"
    "${HEADER_LOCATION}/SharedPtr.h"
    "${HEADER_LOCATION}/TimeValue.h"
    "${HEADER_LOCATION}/TimeValueC.h"
    "${HEADER_LOCATION}/UniquePtr.h"
    "${HEADER_LOCATION}/Util.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h"
    "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")
source_group(API FILES ${API})

set(SOURCE
    AnyMap.cpp
    TimeValueC.cpp
    PlatformConfig.h.in
    Verbosity.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

add_library(ogvrUtil ${API} ${SOURCE})

set_target_properties(ogvrUtil PROPERTIES
    FOLDER "OGVR Libraries"
    VERSION ${OGVR_VERSION}
    SOVERSION 0)

generate_export_header(ogvrUtil
    BASE_NAME OGVR_UTIL
    EXPORT_FILE_NAME Export.h)

target_include_directories(ogvrUtil
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)

target_include_directories(ogvrUtil
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_compile_options(ogvrUtil PRIVATE ${OGVR_CXX11_FLAGS})

# TODO VRPN used in this lib only for gettimeofday wrapper: do we need it?
target_link_libraries(ogvrUtil PRIVATE vendored-vrpn)

###
# C++ util library/headers
###
add_library(ogvrUtilCpp INTERFACE)
target_link_libraries(ogvrUtilCpp INTERFACE ogvrUtil)
target_include_directories(ogvrUtilCpp
    INTERFACE ${Boost_INCLUDE_DIRS})

target_compile_options(ogvrUtilCpp
    INTERFACE ${OGVR_CXX11_FLAGS})

###
# Install library and headers
###
install(TARGETS ogvrUtil ogvrUtilCpp
    EXPORT ogvrTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES
    ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/Util
    COMPONENT
    Devel)


