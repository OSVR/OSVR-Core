osvr_setup_lib_vars(Server)

set(API
    "${HEADER_LOCATION}/Server.h"
    "${HEADER_LOCATION}/ServerPtr.h"
    "${HEADER_LOCATION}/RegisterShutdownHandler.h"
    "${HEADER_LOCATION}/RegisterShutdownHandlerPOSIXSignal.h"
    "${HEADER_LOCATION}/RegisterShutdownHandlerWin32.h")

set(SOURCE
    Server.cpp
    ServerImpl.cpp
    ServerImpl.h)

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

if(WIN32 AND NOT MSVC AND Boost_USE_STATIC_LIBS)
    # Work around a foolish insistence to use DLLs on MXE that don't exist.
    target_compile_definitions(${LIBNAME_FULL}
        PRIVATE
        BOOST_THREAD_USE_LIB)
endif()

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrConnection
    osvrPluginHost
    PRIVATE
    osvrUtilCpp
    util-runloopmanager)

