osvr_setup_lib_vars(Client)

set(API
    "${HEADER_LOCATION}/ClientContext.h"
    "${HEADER_LOCATION}/ClientContext_fwd.h"
    "${HEADER_LOCATION}/ClientInterface.h"
    "${HEADER_LOCATION}/ClientInterfacePtr.h"
    "${HEADER_LOCATION}/CreateContext.h"
    "${HEADER_LOCATION}/PathElementTypes.h"
    "${HEADER_LOCATION}/PathTree.h"
    "${HEADER_LOCATION}/TreeNode.h"
    "${HEADER_LOCATION}/TreeNode_fwd.h")

set(SOURCE
    ClientContext.cpp
    ClientInterface.cpp
    CreateContext.cpp
    PathElementTypes.cpp
    PathTree.cpp
    TreeNode.cpp
    VRPNContext.cpp
    VRPNContext.h)

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
    osvrUtilCpp
    vendored-vrpn)
