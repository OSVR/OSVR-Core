osvr_setup_lib_vars(Routing)

set(API
    "${HEADER_LOCATION}/PathElementTypes.h"
    "${HEADER_LOCATION}/PathElementTypes_fwd.h"
    "${HEADER_LOCATION}/PathTree.h")

set(SOURCE
    PathElementTypes.cpp
    PathTree.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    osvrUtilCpp)
