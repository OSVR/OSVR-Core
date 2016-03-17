osvr_setup_lib_vars(Client)

set(API
    "${HEADER_LOCATION}/ClientInterfaceObjectManager.h"
    "${HEADER_LOCATION}/CreateContext.h"
    "${HEADER_LOCATION}/DisplayConfig.h"
    "${HEADER_LOCATION}/DisplayInput.h"
    "${HEADER_LOCATION}/HandlerContainer.h"
    "${HEADER_LOCATION}/InternalInterfaceOwner.h"
    "${HEADER_LOCATION}/LocateServer.h"
    "${HEADER_LOCATION}/InterfaceTree.h"
    "${HEADER_LOCATION}/RemoteHandler.h"
    "${HEADER_LOCATION}/RemoteHandlerFactory.h"
    "${HEADER_LOCATION}/RenderManagerConfig.h"
    "${HEADER_LOCATION}/Viewer.h"
    "${HEADER_LOCATION}/Viewers.h"
    "${HEADER_LOCATION}/ViewerEye.h"
    "${HEADER_LOCATION}/ViewerEyeSurface.h")

set(SOURCE
    AnalogRemoteFactory.cpp
    AnalogRemoteFactory.h
    AnalysisClientContext.cpp
    AnalysisClientContext.h
    ButtonRemoteFactory.cpp
    ButtonRemoteFactory.h
    ClientInterfaceObjectManager.cpp
    CreateContext.cpp
    DirectionRemoteFactory.cpp
    DirectionRemoteFactory.h
    DisplayConfig.cpp
    DisplayDescriptorSchema1.cpp
    DisplayDescriptorSchema1.h
    DisplayInput.cpp
    EyeTrackerRemoteFactory.cpp
    EyeTrackerRemoteFactory.h
    ImagingRemoteFactory.cpp
    ImagingRemoteFactory.h
    InterfaceTree.cpp
    Location2DRemoteFactory.cpp
    Location2DRemoteFactory.h
    LocomotionRemoteFactory.cpp
    LocomotionRemoteFactory.h
    PureClientContext.cpp
    PureClientContext.h
    RemoteHandler.cpp
    RemoteHandlerFactory.cpp
    RemoteHandlerInternals.h
    TrackerRemoteFactory.cpp
    TrackerRemoteFactory.h
    Viewer.cpp
    Viewers.cpp
    ViewerEye.cpp
    ViewerEyeSurface.cpp
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
    JsonCpp::JsonCpp
    vendored-vrpn
    eigen-headers)

install(FILES
    ${DISPLAY_JSON}
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)
