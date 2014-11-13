ogvr_setup_lib_vars(PluginHost)

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

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OGVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    libfunctionality::functionality
    ogvrUtilCpp)

###
# Grab DLLs please.
###
if(WIN32)
    add_custom_command(TARGET ${LIBNAME_FULL} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libfunctionality::functionality> $<TARGET_FILE_DIR:${LIBNAME_FULL}>
        COMMENT Copying required DLL
        VERBATIM)
endif()
