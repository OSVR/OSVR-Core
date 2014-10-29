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
    "${HEADER_LOCATION}/CallbackWrapper.h"
    "${HEADER_LOCATION}/GenericDeleter.h"
    "${HEADER_LOCATION}/ResourcePath.h"
    "${HEADER_LOCATION}/SharedPtr.h"
    "${HEADER_LOCATION}/TimeValue.h"
    "${HEADER_LOCATION}/TimeValueC.h"
    "${HEADER_LOCATION}/UniquePtr.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h"
    "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")
source_group(inc\\ogvr\\Util FILES ${API})

set(SOURCE
    CallbackWrapper.cpp
    ResourcePath.cpp
    TimeValueC.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")
source_group(src\\ogvr\\Util FILES ${SOURCE})

add_library(ogvrUtil ${API} ${SOURCE})

generate_export_header(ogvrUtil
    BASE_NAME OGVR_UTIL
    EXPORT_FILE_NAME Export.h)

target_include_directories(ogvrUtil
    PUBLIC
    $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)

target_include_directories(ogvrUtil
    PUBLIC
    ${Boost_INCLUDE_DIR})

# TODO: make two targets here too so that we can explain that C++ clients
# of this lib require C++11 flags but not C clients?
target_compile_options(ogvrUtil PRIVATE ${OGVR_CXX11_FLAGS})

###
# Install library and headers
###
install(TARGETS ogvrUtil
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


