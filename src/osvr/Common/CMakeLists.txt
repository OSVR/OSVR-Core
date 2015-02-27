osvr_setup_lib_vars(Common)

include(CheckIncludeFile)

check_include_file(byteswap.h OSVR_HAVE_BYTESWAP_H)
configure_file(ConfigByteSwapping.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(API
    "${HEADER_LOCATION}/AlignmentPadding.h"
    "${HEADER_LOCATION}/BaseMessageTraits.h"
    "${HEADER_LOCATION}/Buffer.h"
    "${HEADER_LOCATION}/BufferTraits.h"
    "${HEADER_LOCATION}/Buffer_fwd.h"
    "${HEADER_LOCATION}/Common.h"
    "${HEADER_LOCATION}/ConnectionWrapper.h"
    "${HEADER_LOCATION}/CreateDevice.h"
    "${HEADER_LOCATION}/BaseDevice.h"
    "${HEADER_LOCATION}/BaseDevicePtr.h"
    "${HEADER_LOCATION}/DeviceComponent.h"
    "${HEADER_LOCATION}/DeviceComponentPtr.h"
    "${HEADER_LOCATION}/Endianness.h"
    "${HEADER_LOCATION}/ImagingComponent.h"
    "${HEADER_LOCATION}/MessageHandler.h"
    "${HEADER_LOCATION}/MessageRegistration.h"
    "${HEADER_LOCATION}/RawMessageType.h"
    "${HEADER_LOCATION}/RawSenderType.h"
    "${HEADER_LOCATION}/Serialization.h"
    "${HEADER_LOCATION}/SerializationTags.h"
    "${HEADER_LOCATION}/SerializationTraits.h"
    "${HEADER_LOCATION}/SystemComponent.h"
    "${HEADER_LOCATION}/SystemComponent_fwd.h"
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(SOURCE
    BaseDevice.cpp
    Common.cpp
    CreateDevice.cpp
    DeviceComponent.cpp
    DeviceWrapper.cpp
    DeviceWrapper.h
    ImagingComponent.cpp
    MessageHandler.cpp
    MessageRegistration.cpp
    RawMessageType.cpp
    RawSenderType.cpp
    Serialization.cpp
    SystemComponent.cpp)

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
    osvrUtilCpp
    jsoncpp_lib
    PRIVATE
    opencv_core
    vendored-vrpn
    eigen-headers)

osvr_delayload_opencv(${LIBNAME_FULL} opencv_core)
