osvr_setup_lib_vars(Common)

include(CheckIncludeFile)
include(CheckCSourceCompiles)

check_include_file(intrin.h OSVR_HAVE_INTRIN_H)
check_include_file(byteswap.h OSVR_HAVE_BYTESWAP_H)
check_c_source_compiles("#include <intrin.h>\nint main() {return _byteswap_ushort(0x1234) + _byteswap_ulong(0x12345678) + (int)_byteswap_uint64(0x0123456789abcdef);}" OSVR_HAVE_WORKING_MS_BYTESWAPS)
check_c_source_compiles("#include <byteswap.h>\nint main() {return bswap16(0x1234);}" OSVR_HAVE_WORKING_BSWAP)
check_c_source_compiles("#include <byteswap.h>\nint main() {return bswap_16(0x1234);}" OSVR_HAVE_WORKING_BSWAP_UNDERSCORE)
check_c_source_compiles("#include <byteswap.h>\nint main() {return __bswap_16(0x1234);}" OSVR_HAVE_WORKING_UNDERSCORES_BSWAP)
configure_file(ConfigByteSwapping.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(API
    "${HEADER_LOCATION}/AddDevice.h"
    "${HEADER_LOCATION}/AlignmentPadding.h"
    "${HEADER_LOCATION}/ApplyPathNodeVisitor.h"
    "${HEADER_LOCATION}/BaseDevice.h"
    "${HEADER_LOCATION}/BaseDevicePtr.h"
    "${HEADER_LOCATION}/BaseMessageTraits.h"
    "${HEADER_LOCATION}/Buffer.h"
    "${HEADER_LOCATION}/BufferTraits.h"
    "${HEADER_LOCATION}/Buffer_fwd.h"
    "${HEADER_LOCATION}/CallbackType.h"
    "${HEADER_LOCATION}/ChangeOfBasis.h"
    "${HEADER_LOCATION}/ClientContext.h"
    "${HEADER_LOCATION}/ClientContext_fwd.h"
    "${HEADER_LOCATION}/ClientInterface.h"
    "${HEADER_LOCATION}/ClientInterfacePtr.h"
    "${HEADER_LOCATION}/Common.h"
    "${HEADER_LOCATION}/CommonComponent.h"
    "${HEADER_LOCATION}/CommonComponent_fwd.h"
    "${HEADER_LOCATION}/ConnectionWrapper.h"
    "${HEADER_LOCATION}/CreateDevice.h"
    "${HEADER_LOCATION}/DeduplicatingFunctionWrapper.h"
    "${HEADER_LOCATION}/DegreesToRadians.h"
    "${HEADER_LOCATION}/DeviceComponent.h"
    "${HEADER_LOCATION}/DeviceComponentPtr.h"
    "${HEADER_LOCATION}/Endianness.h"
    "${HEADER_LOCATION}/GetEnvironmentVariable.h"
    "${HEADER_LOCATION}/ImagingComponent.h"
    "${HEADER_LOCATION}/IntegerByteSwap.h"
    "${HEADER_LOCATION}/InterfaceCallbacks.h"
    "${HEADER_LOCATION}/InterfaceList.h"
    "${HEADER_LOCATION}/InterfaceState.h"
    "${HEADER_LOCATION}/IPCRingBuffer.h"
    "${HEADER_LOCATION}/JSONEigen.h"
    "${HEADER_LOCATION}/JSONSerializationTags.h"
    "${HEADER_LOCATION}/JSONTransformVisitor.h"
    "${HEADER_LOCATION}/MessageHandler.h"
    "${HEADER_LOCATION}/MessageRegistration.h"
    "${HEADER_LOCATION}/NetworkClassOfService.h"
    "${HEADER_LOCATION}/NetworkingSupport.h"
    "${HEADER_LOCATION}/OriginalSource.h"
    "${HEADER_LOCATION}/ParseAlias.h"
    "${HEADER_LOCATION}/PathElementTools.h"
    "${HEADER_LOCATION}/PathElementTypes.h"
    "${HEADER_LOCATION}/PathElementTypes_fwd.h"
    "${HEADER_LOCATION}/PathNode.h"
    "${HEADER_LOCATION}/PathNode_fwd.h"
    "${HEADER_LOCATION}/PathTree.h"
    "${HEADER_LOCATION}/PathTreeFull.h"
    "${HEADER_LOCATION}/PathTreeSerialization.h"
    "${HEADER_LOCATION}/PathTree_fwd.h"
    "${HEADER_LOCATION}/ProcessAliasesFromJSON.h"
    "${HEADER_LOCATION}/ProcessDeviceDescriptor.h"
    "${HEADER_LOCATION}/RawMessageType.h"
    "${HEADER_LOCATION}/RawSenderType.h"
    "${HEADER_LOCATION}/ReportFromCallback.h"
    "${HEADER_LOCATION}/ReportMap.h"
    "${HEADER_LOCATION}/ReportState.h"
    "${HEADER_LOCATION}/ReportStateTraits.h"
    "${HEADER_LOCATION}/ReportTraits.h"
    "${HEADER_LOCATION}/ReportTypes.h"
    "${HEADER_LOCATION}/ResolveFullTree.h"
    "${HEADER_LOCATION}/ResolveTreeNode.h"
    "${HEADER_LOCATION}/RouteContainer.h"
    "${HEADER_LOCATION}/RoutingConstants.h"
    "${HEADER_LOCATION}/RoutingExceptions.h"
    "${HEADER_LOCATION}/RoutingKeys.h"
    "${HEADER_LOCATION}/Serialization.h"
    "${HEADER_LOCATION}/SerializationTags.h"
    "${HEADER_LOCATION}/SerializationTraits.h"
    "${HEADER_LOCATION}/StateType.h"
    "${HEADER_LOCATION}/SystemComponent.h"
    "${HEADER_LOCATION}/SystemComponent_fwd.h"
    "${HEADER_LOCATION}/Transform.h"
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h")

set(SOURCE
    AddDevice.cpp
    BaseDevice.cpp
    ClientContext.cpp
    ClientInterface.cpp
    Common.cpp
    CommonComponent.cpp
    ConfigByteSwapping.h.cmake_in
    CreateDevice.cpp
    DeviceComponent.cpp
    DeviceWrapper.cpp
    DeviceWrapper.h
    GetEnvironmentVariable.cpp
    GetJSONStringFromTree.h
    ImagingComponent.cpp
    IPCRingBuffer.cpp
    IPCRingBufferResults.h
    IPCRingBufferSharedObjects.h
    JSONTransformVisitor.cpp
    MessageHandler.cpp
    MessageRegistration.cpp
    NetworkClassOfService.cpp
    NetworkingSupport.cpp
    OriginalSource.cpp
    ParseAlias.cpp
    PathElementSerialization.h
    PathElementSerializationDescriptions.h
    PathElementTools.cpp
    PathElementTypes.cpp
    PathNode.cpp
    PathParseAndRetrieve.h
    PathTree.cpp
    PathTreeSerialization.cpp
    ProcessAliasesFromJSON.cpp
    ProcessDeviceDescriptor.cpp
    RawMessageType.cpp
    RawSenderType.cpp
    ResolveFullTree.cpp
    ResolveTreeNode.cpp
    RouteContainer.cpp
    RoutingConstants.cpp
    RoutingKeys.cpp
    Serialization.cpp
    SharedMemory.h
    SharedMemoryObjectWithMutex.h
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
    ${Boost_INCLUDE_DIRS}
    PRIVATE
    ${OpenCV_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrUtilCpp
    PRIVATE
    jsoncpp_lib
    opencv_core
    vendored-vrpn
    eigen-headers
    ${OSVR_CODECVT_LIBRARIES})

osvr_delayload_opencv(${LIBNAME_FULL} opencv_core)

###
# Grab DLLs please.
###
if(OSVR_COPY_OPENCV)
    # This copies the DLLs used by the core, as well as by any enabled in-tree plugins.
    foreach(opencvlib ${OPENCV_MODULES_USED})
        osvr_copy_dep(${LIBNAME_FULL} ${opencvlib})
    endforeach()
endif()
