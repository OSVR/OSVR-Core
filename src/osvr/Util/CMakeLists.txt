###
# Internal Verbosity Header
###
if(BUILD_DEV_VERBOSE)
    set(OSVR_UTIL_DEV_VERBOSE TRUE)
endif()
configure_file(Verbosity.h.in "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

###
# Platform Config Header
###
include(CheckStructHasMember)
check_struct_has_member("struct timeval" tv_sec sys/time.h OSVR_HAVE_STRUCT_TIMEVAL_IN_SYS_TIME_H LANGUAGE C)
check_struct_has_member("struct timeval" tv_sec winsock2.h OSVR_HAVE_STRUCT_TIMEVAL_IN_WINSOCK2_H LANGUAGE C)
configure_file(PlatformConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")

###
# Library build
###
osvr_setup_lib_vars(Util)

set(API
    "${HEADER_LOCATION}/APIBaseC.h"
    "${HEADER_LOCATION}/AnnotationMacrosC.h"
    "${HEADER_LOCATION}/AnyMap.h"
    "${HEADER_LOCATION}/AnyMap_fwd.h"
    "${HEADER_LOCATION}/BoolC.h"
    "${HEADER_LOCATION}/BoostIsCopyConstructible.h"
    "${HEADER_LOCATION}/CallbackWrapper.h"
    "${HEADER_LOCATION}/ChannelCountC.h"
    "${HEADER_LOCATION}/ClientOpaqueTypesC.h"
    "${HEADER_LOCATION}/ClientReportTypesC.h"
    "${HEADER_LOCATION}/DeviceCallbackTypesC.h"
    "${HEADER_LOCATION}/EigenCoreGeometry.h"
    "${HEADER_LOCATION}/EigenInterop.h"
    "${HEADER_LOCATION}/GenericCaller.h"
    "${HEADER_LOCATION}/GenericDeleter.h"
    "${HEADER_LOCATION}/GuardInterface.h"
    "${HEADER_LOCATION}/GuardInterfaceDummy.h"
    "${HEADER_LOCATION}/ImagingReportTypesC.h"
    "${HEADER_LOCATION}/MessageKeys.h"
    "${HEADER_LOCATION}/MSStdIntC.h"
    "${HEADER_LOCATION}/MacroToolsC.h"
    "${HEADER_LOCATION}/NumberTypeManipulation.h"
    "${HEADER_LOCATION}/OpenCVTypeDispatch.h"
    "${HEADER_LOCATION}/PluginCallbackTypesC.h"
    "${HEADER_LOCATION}/PluginRegContextC.h"
    "${HEADER_LOCATION}/Pose3C.h"
    "${HEADER_LOCATION}/QuaternionC.h"
    "${HEADER_LOCATION}/QuatlibInteropC.h"
    "${HEADER_LOCATION}/ResetPointerList.h"
    "${HEADER_LOCATION}/ResourcePath.h"
    "${HEADER_LOCATION}/ReturnCodesC.h"
    "${HEADER_LOCATION}/SharedPtr.h"
    "${HEADER_LOCATION}/StdInt.h"
    "${HEADER_LOCATION}/TimeValue.h"
    "${HEADER_LOCATION}/TimeValueC.h"
    "${HEADER_LOCATION}/TimeValue_fwd.h"
    "${HEADER_LOCATION}/TreeNode.h"
    "${HEADER_LOCATION}/TreeNode_fwd.h"
    "${HEADER_LOCATION}/UniquePtr.h"
    "${HEADER_LOCATION}/Util.h"
    "${HEADER_LOCATION}/Vec3C.h"
    "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")

set(SOURCE
    AnyMap.cpp
    GuardInterface.cpp
    TimeValueC.cpp
    MessageKeys.cpp
    PlatformConfig.h.in
    Verbosity.h.in
    ClientCallbackTypesC.h.in
    ClientCallbackTypesCSnippet.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/ClientCallbackTypesC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PRIVATE
    ${OSVR_CXX11_FLAGS})

osvr_generate_for_report_types(ClientCallbackTypesC.h.in
    ClientCallbackTypesCSnippet.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/ClientCallbackTypesC.h")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ClientCallbackTypesC.h"
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/osvr/${LIBNAME}
    COMPONENT Devel)

# TODO VRPN used in this lib only for gettimeofday wrapper: do we need it?
target_link_libraries(${LIBNAME_FULL} PRIVATE vendored-vrpn)

###
# C++ util library/headers
###
osvr_add_cpp_interface_library()
# In an installed version, we search and add the dependency in the config script
# since we don't need the same version in the same place.
target_include_directories(${LIBNAME_INTERFACE}
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_options(${LIBNAME_INTERFACE}
    INTERFACE ${OSVR_CXX11_FLAGS})
