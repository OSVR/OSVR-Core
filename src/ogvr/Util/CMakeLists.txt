###
# Internal Verbosity Header
###
if(BUILD_DEV_VERBOSE)
    set(OGVR_UTIL_DEV_VERBOSE TRUE)
endif()
configure_file(Verbosity.h.in "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

###
# Platform Config Header
###
include(CheckStructHasMember)
check_struct_has_member("struct timeval" tv_sec sys/time.h OGVR_HAVE_STRUCT_TIMEVAL_IN_SYS_TIME_H LANGUAGE C)
check_struct_has_member("struct timeval" tv_sec winsock2.h OGVR_HAVE_STRUCT_TIMEVAL_IN_WINSOCK2_H LANGUAGE C)
configure_file(PlatformConfig.h.in "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")

###
# Library build
###
ogvr_setup_lib_vars(Util)

set(API
    "${HEADER_LOCATION}/APIBaseC.h"
    "${HEADER_LOCATION}/AnnotationMacrosC.h"
    "${HEADER_LOCATION}/AnyMap.h"
    "${HEADER_LOCATION}/AnyMap_fwd.h"
    "${HEADER_LOCATION}/BoostIsCopyConstructible.h"
    "${HEADER_LOCATION}/CallbackWrapper.h"
    "${HEADER_LOCATION}/ClientCallbackTypesC.h"
    "${HEADER_LOCATION}/ClientOpaqueTypes.h"
    "${HEADER_LOCATION}/DeviceCallbackTypesC.h"
    "${HEADER_LOCATION}/GenericCaller.h"
    "${HEADER_LOCATION}/GenericDeleter.h"
    "${HEADER_LOCATION}/MSStdIntC.h"
    "${HEADER_LOCATION}/MacroToolsC.h"
    "${HEADER_LOCATION}/PluginCallbackTypesC.h"
    "${HEADER_LOCATION}/PluginRegContextC.h"
    "${HEADER_LOCATION}/ResetPointerList.h"
    "${HEADER_LOCATION}/ResourcePath.h"
    "${HEADER_LOCATION}/ReturnCodesC.h"
    "${HEADER_LOCATION}/SharedPtr.h"
    "${HEADER_LOCATION}/StdInt.h"
    "${HEADER_LOCATION}/TimeValue.h"
    "${HEADER_LOCATION}/TimeValueC.h"
    "${HEADER_LOCATION}/TimeValue_fwd.h"
    "${HEADER_LOCATION}/UniquePtr.h"
    "${HEADER_LOCATION}/Util.h"
    "${CMAKE_CURRENT_BINARY_DIR}/PlatformConfig.h")

set(SOURCE
    AnyMap.cpp
    TimeValueC.cpp
    PlatformConfig.h.in
    Verbosity.h.in
    "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

ogvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OGVR_VERSION}
    SOVERSION 0)


target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})


target_compile_options(${LIBNAME_FULL}
    PRIVATE
    ${OGVR_CXX11_FLAGS})

# TODO VRPN used in this lib only for gettimeofday wrapper: do we need it?
target_link_libraries(${LIBNAME_FULL} PRIVATE vendored-vrpn)

###
# C++ util library/headers
###
ogvr_add_cpp_interface_library()
target_include_directories(${LIBNAME_FULL_CPP}
    INTERFACE ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL_CPP}
    INTERFACE ${OGVR_CXX11_FLAGS})
