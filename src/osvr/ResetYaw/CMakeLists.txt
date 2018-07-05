osvr_setup_lib_vars(ResetYaw)

set(API "${HEADER_LOCATION}/ResetYaw.h")

set(SOURCE
    ResetYaw.cpp
    ClientMainloop.h
    ClientMainloopThread.h)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
     INTERFACE
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(${LIBNAME_FULL}
    PRIVATE
    osvrUtil
    JsonCpp::JsonCpp
    osvrClientKitCpp
    osvrClient
    osvrCommon
    eigen-headers
    boost_thread)
