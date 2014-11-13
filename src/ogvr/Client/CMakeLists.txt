ogvr_setup_lib_vars(Client)

set(API
    "${HEADER_LOCATION}/ClientContext.h"
    "${HEADER_LOCATION}/ClientContext_fwd.h"
    "${HEADER_LOCATION}/ClientInterface.h"
    "${HEADER_LOCATION}/ClientInterfacePtr.h"
    "${HEADER_LOCATION}/CreateContext.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(SOURCE
    ClientContext.cpp
    ClientInterface.cpp
    CreateContext.cpp
    VRPNContext.cpp
    VRPNContext.h)

ogvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    ogvrUtilCpp
    vendored-vrpn
    qvrpn-headers)
