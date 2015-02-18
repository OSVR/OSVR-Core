osvr_setup_lib_vars(PluginHost)

set(API
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext_fwd.h"
    "${HEADER_LOCATION}/PluginSpecificRegistrationContext.h"
    "${HEADER_LOCATION}/PluginRegPtr.h"
    "${HEADER_LOCATION}/RegistrationContext_fwd.h"
    "${HEADER_LOCATION}/RegistrationContext.h"
    "${HEADER_LOCATION}/SearchPath.h")

set(SOURCE
    BinaryLocation.cpp
    BinaryLocation.h
    PluginSpecificRegistrationContext.cpp
    PluginSpecificRegistrationContextImpl.cpp
    PluginSpecificRegistrationContextImpl.h
    RegistrationContext.cpp
    SearchPath.cpp)

configure_file(PathConfig.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/PathConfig.h")

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
    osvrUtilCpp
    PRIVATE
    boost_filesystem)

###
# Grab DLLs please.
###
if(WIN32)
    add_custom_command(TARGET ${LIBNAME_FULL} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libfunctionality::functionality> $<TARGET_FILE_DIR:${LIBNAME_FULL}>
        COMMENT "Copying required DLL"
        VERBATIM)
endif()
if(WIN32)
    install(FILES $<TARGET_FILE:libfunctionality::functionality>
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
else()
    install(FILES $<TARGET_FILE:libfunctionality::functionality>
        DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime)
endif()
