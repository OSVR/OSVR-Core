if(BUILD_DEV_VERBOSE)
    set(OGVR_UTIL_DEV_VERBOSE TRUE)
endif()
configure_file(Verbosity.h.in "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/Util")

set(API
    "${HEADER_LOCATION}/CallbackWrapper.h"
    "${HEADER_LOCATION}/GenericDeleter.h"
    "${HEADER_LOCATION}/ResourcePath.h"
    "${HEADER_LOCATION}/SharedPtr.h"
    "${HEADER_LOCATION}/TimeValue.h"
    "${HEADER_LOCATION}/TimeValueC.h"
    "${HEADER_LOCATION}/UniquePtr.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
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


