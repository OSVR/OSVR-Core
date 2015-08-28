osvr_setup_lib_vars(USBSerial)
set(API
    "${HEADER_LOCATION}/USBSerialDevice.h"
    "${HEADER_LOCATION}/USBSerialEnum.h")

set(SOURCE
    USBSerialDevInfo.h
    USBSerialDevInfo.cpp
    USBSerialDevInfo_Linux.h
    USBSerialDevInfo_Windows.h
    USBSerialEnum.cpp
    USBSerialEnumImpl.h
    USBSerialEnumImpl.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrUtilCpp
    PRIVATE
    boost_filesystem
    ${OSVR_CODECVT_LIBRARIES})
if(WIN32)
    target_link_libraries(${LIBNAME_FULL}
        PRIVATE
        wbemuuid
        com_smart_pointer)
endif()
