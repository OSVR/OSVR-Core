osvr_setup_lib_vars(Common)

include(CheckIncludeFile)

check_include_file(byteswap.h OSVR_HAVE_BYTESWAP_H)
configure_file(ConfigByteSwapping.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(API
    "${HEADER_LOCATION}/Buffer.h"
    "${HEADER_LOCATION}/Endianness.h"
    "${HEADER_LOCATION}/MessageRegistration.h"
    "${HEADER_LOCATION}/Serialization.h"
    "${HEADER_LOCATION}/SerializationTraits.h"
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(SOURCE
    MessageRegistration.cpp
    Serialization.cpp)

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
    jsoncpp_lib
    PRIVATE
    eigen-headers)
