osvr_setup_lib_vars(Client)

set(API
    "${HEADER_LOCATION}/CallbackType.h"
    "${HEADER_LOCATION}/ClientContext.h"
    "${HEADER_LOCATION}/ClientContext_fwd.h"
    "${HEADER_LOCATION}/ClientInterface.h"
    "${HEADER_LOCATION}/ClientInterfacePtr.h"
    "${HEADER_LOCATION}/CreateContext.h"
    "${HEADER_LOCATION}/InterfaceCallbacks.h"
    "${HEADER_LOCATION}/InterfaceState.h"
    "${HEADER_LOCATION}/ReportFromCallback.h"
    "${HEADER_LOCATION}/ReportState.h"
    "${HEADER_LOCATION}/ReportStateTraits.h"
    "${HEADER_LOCATION}/ReportTraits.h"
    "${HEADER_LOCATION}/ReportTypes.h"
    "${HEADER_LOCATION}/ReportTypeWrapper.h"
    "${HEADER_LOCATION}/ReportMap.h"
    "${HEADER_LOCATION}/StateType.h")

set(SOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/display_json.h"
    ClientContext.cpp
    ClientInterface.cpp
    CreateContext.cpp
    ImagingRouter.h
    PureClientContext.h
    PureClientContext.cpp
    RouterTransforms.h
    RouterPredicates.h
    VRPNContext.cpp
    VRPNContext.h
    VRPNAnalogRouter.h
    VRPNButtonRouter.h
    VRPNTrackerRouter.h)

set(DISPLAY_JSON display-HDK.json)
osvr_convert_json(display_json ${DISPLAY_JSON} "${CMAKE_CURRENT_BINARY_DIR}/display_json.h")

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