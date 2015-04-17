osvr_setup_lib_vars(Client)

set(API
    "${HEADER_LOCATION}/CreateContext.h")

set(SOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/display_json.h"
    AnalogRemoteFactory.cpp
    AnalogRemoteFactory.h
    ButtonRemoteFactory.cpp
    ButtonRemoteFactory.h
    CreateContext.cpp
    HandlerContainer.h
    ImagingRemoteFactory.cpp
    ImagingRemoteFactory.h
    InterfaceTree.cpp
    InterfaceTree.h
    PureClientContext.cpp
    PureClientContext.h
    RemoteHandler.cpp
    RemoteHandler.h
    RemoteHandlerFactory.h
    RouterPredicates.h
    RouterTransforms.h
    TrackerRemoteFactory.cpp
    TrackerRemoteFactory.h
    VRPNConnectionCollection.cpp
    VRPNConnectionCollection.h)


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
    osvrCommon
    jsoncpp_lib
    vendored-vrpn
    eigen-headers)

install(FILES
    ${DISPLAY_JSON}
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)
