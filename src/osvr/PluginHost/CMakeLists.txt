osvr_setup_lib_vars(PluginHost)

set(API
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext_fwd.h"
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext.h"
    "${HEADER_LOCATION}/PluginRegPtr.h"
    "${HEADER_LOCATION}/RegistrationContext_fwd.h"
    "${HEADER_LOCATION}/RegistrationContext.h")

set(SOURCE
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContextImpl.cpp
    PluginSpecificRegistrationContextImpl.h
    RegistrationContext.cpp)

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

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    libfunctionality::functionality
    osvrUtilCpp)

###
# Grab DLLs please.
###
osvr_copy_dep(${LIBNAME_FULL} libfunctionality::functionality)