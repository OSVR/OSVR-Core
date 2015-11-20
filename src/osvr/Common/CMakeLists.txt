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

if(ETWPROVIDERS_FOUND)
    option(BUILD_WITH_TRACING "Build with high-performance tracing support built-in?" OFF)
else()
    set(BUILD_WITH_TRACING OFF)
endif()
if(BUILD_WITH_TRACING)
    if(ETWPROVIDERS_FOUND)
        set(OSVR_COMMON_TRACING_ENABLED ON)
        set(OSVR_COMMON_TRACING_ETW ON)
    endif()
endif()

option(OSVR_COMMON_IN_PROCESS_IMAGING "Option to switch from shared-memory imaging messages to use only in-process memory messages. Requires single-process client/server." OFF)

mark_as_advanced(OSVR_COMMON_IN_PROCESS_IMAGING)

configure_file(TracingConfig.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/TracingConfig.h")

configure_file(ImagingComponentConfig.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/ImagingComponentConfig.h")

set(API
    "${HEADER_LOCATION}/AddDevice.h"
    "${HEADER_LOCATION}/AliasProcessor.h"
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
    "${HEADER_LOCATION}/ClientInterfaceFactory.h"
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
    "${HEADER_LOCATION}/DirectionComponent.h"
    "${HEADER_LOCATION}/Endianness.h"
    "${HEADER_LOCATION}/EyeTrackerComponent.h"
    "${HEADER_LOCATION}/GeneralizedTransform.h"
    "${HEADER_LOCATION}/GetEnvironmentVariable.h"
    "${HEADER_LOCATION}/ImagingComponent.h"
    "${CMAKE_CURRENT_BINARY_DIR}/ImagingComponentConfig.h"
    "${HEADER_LOCATION}/IntegerByteSwap.h"
    "${HEADER_LOCATION}/InterfaceCallbacks.h"
    "${HEADER_LOCATION}/InterfaceList.h"
    "${HEADER_LOCATION}/InterfaceState.h"
    "${HEADER_LOCATION}/IPCRingBuffer.h"
    "${HEADER_LOCATION}/JSONEigen.h"
    "${HEADER_LOCATION}/JSONHelpers.h"
    "${HEADER_LOCATION}/JSONSerializationTags.h"
    "${HEADER_LOCATION}/JSONTimestamp.h"
    "${HEADER_LOCATION}/JSONTransformVisitor.h"
    "${HEADER_LOCATION}/Location2DComponent.h"
    "${HEADER_LOCATION}/LocomotionComponent.h"
    "${HEADER_LOCATION}/MessageHandler.h"
    "${HEADER_LOCATION}/MessageRegistration.h"
    "${HEADER_LOCATION}/NetworkClassOfService.h"
    "${HEADER_LOCATION}/NetworkingSupport.h"
    "${HEADER_LOCATION}/NormalizeDeviceDescriptor.h"
    "${HEADER_LOCATION}/OriginalSource.h"
    "${HEADER_LOCATION}/ParseAlias.h"
    "${HEADER_LOCATION}/PathElementTools.h"
    "${HEADER_LOCATION}/PathElementTypes.h"
    "${HEADER_LOCATION}/PathElementTypes_fwd.h"
    "${HEADER_LOCATION}/PathNode.h"
    "${HEADER_LOCATION}/PathNode_fwd.h"
    "${HEADER_LOCATION}/PathTree.h"
    "${HEADER_LOCATION}/PathTreeFull.h"
    "${HEADER_LOCATION}/PathTreeObserver.h"
    "${HEADER_LOCATION}/PathTreeObserverPtr.h"
    "${HEADER_LOCATION}/PathTreeOwner.h"
    "${HEADER_LOCATION}/PathTreeSerialization.h"
    "${HEADER_LOCATION}/PathTree_fwd.h"
    "${HEADER_LOCATION}/ProcessDeviceDescriptor.h"
    "${HEADER_LOCATION}/RawMessageType.h"
    "${HEADER_LOCATION}/RawSenderType.h"
    "${HEADER_LOCATION}/RegisteredStringMap.h"
    "${HEADER_LOCATION}/ReportFromCallback.h"
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
    "${HEADER_LOCATION}/Tracing.h"
    "${HEADER_LOCATION}/TrackerSensorInfo.h"
    "${HEADER_LOCATION}/Transform.h"
    "${HEADER_LOCATION}/Transform_fwd.h"
    "${CMAKE_CURRENT_BINARY_DIR}/ConfigByteSwapping.h"
    "${CMAKE_CURRENT_BINARY_DIR}/TracingConfig.h")

set(SOURCE
    AddDevice.cpp
    AliasProcessor.cpp
    BaseDevice.cpp
    ClientContext.cpp
    ClientInterfaceFactory.cpp
    ClientInterface.cpp
    Common.cpp
    CommonComponent.cpp
    ConfigByteSwapping.h.cmake_in
    CreateDevice.cpp
    DeviceComponent.cpp
    DeviceWrapper.cpp
    DeviceWrapper.h
    DirectionComponent.cpp
    EyeTrackerComponent.cpp
    GeneralizedTransform.cpp
    GetEnvironmentVariable.cpp
    GetJSONStringFromTree.h
    ImagingComponent.cpp
    IPCRingBuffer.cpp
    IPCRingBufferResults.h
    IPCRingBufferSharedObjects.h
    JSONTransformVisitor.cpp
    Location2DComponent.cpp
    LocomotionComponent.cpp
    MessageHandler.cpp
    MessageRegistration.cpp
    NetworkClassOfService.cpp
    NetworkingSupport.cpp
    NormalizeDeviceDescriptor.cpp
    OriginalSource.cpp
    ParseAlias.cpp
    PathElementSerialization.h
    PathElementSerializationDescriptions.h
    PathElementTools.cpp
    PathElementTypes.cpp
    PathNode.cpp
    PathParseAndRetrieve.h
    PathTree.cpp
    PathTreeObserver.cpp
    PathTreeOwner.cpp
    PathTreeSerialization.cpp
    ProcessDeviceDescriptor.cpp
    RawMessageType.cpp
    RawSenderType.cpp
    RegisteredStringMap.cpp
    ResolveFullTree.cpp
    ResolveTreeNode.cpp
    RouteContainer.cpp
    RoutingConstants.cpp
    RoutingKeys.cpp
    SharedMemory.h
    SharedMemoryObjectWithMutex.h
    SystemComponent.cpp
    Tracing.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
    PRIVATE
    ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrUtilCpp
    PRIVATE
    jsoncpp_lib
    vendored-vrpn
    eigen-headers
    osvr_cxx11_flags
    ${OSVR_CODECVT_LIBRARIES})

# Target-level dependency: make Util build first, because the Util build contains
# the generated X-Macro header ReportTypesX that is included in ReportTraits.h
add_dependencies(${LIBNAME_FULL} osvrUtil)

if(OSVR_COMMON_TRACING_ETW)
    target_link_libraries(${LIBNAME_FULL} PRIVATE ETWProviders)
    add_custom_command(TARGET ${LIBNAME_FULL} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${ETWPROVIDERS_RUNTIME_LIBRARY}" $<TARGET_FILE_DIR:${LIBNAME_FULL}>
        COMMENT "Copying required DLL for ETWProviders"
        VERBATIM)

    install(FILES
        "${ETWPROVIDERS_RUNTIME_LIBRARY}"
        "${ETWPROVIDERS_MANIFEST}"
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endif()

###
# Grab DLLs please.
###
if(BUILD_WITH_OPENCV AND OSVR_COPY_OPENCV)
    # This copies the DLLs used by any enabled in-tree plugins and examples
    foreach(opencvlib ${OPENCV_MODULES_USED})
        osvr_copy_dep(${LIBNAME_FULL} ${opencvlib})
    endforeach()
endif()
