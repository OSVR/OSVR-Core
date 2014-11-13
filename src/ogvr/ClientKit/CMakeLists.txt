ogvr_setup_lib_vars(ClientKit)

set(API
    "${HEADER_LOCATION}/ClientKitC.h"
    "${HEADER_LOCATION}/ContextC.h"
    "${HEADER_LOCATION}/InterfaceC.h"
    "${HEADER_LOCATION}/InterfaceCallbackC.h"
    "${HEADER_LOCATION}/SystemCallbackC.h")

set(SOURCE
    ClientKitC.cpp
    ContextC.cpp
    InterfaceC.cpp
    InterfaceCallbackC.cpp
    SystemCallbackC.cpp)

ogvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    ogvrClient
    ogvrUtilCpp)
