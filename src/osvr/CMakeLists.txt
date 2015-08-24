include(GenerateExportHeader)

include("${CMAKE_CURRENT_SOURCE_DIR}/DelayLoad.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/GenerateForReportTypes.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/Macros.cmake")

add_subdirectory(Util)
add_subdirectory(Common)

if(BUILD_SERVER)
    add_subdirectory(PluginHost)
    add_subdirectory(Connection)
    add_subdirectory(PluginKit)
    add_subdirectory(Server)
    add_subdirectory(VRPNServer)
    if(BUILD_USBSERIALENUM)
        add_subdirectory(USBSerial)
    endif()
endif()

if(BUILD_CLIENT)
    add_subdirectory(Client)
    add_subdirectory(ClientKit)
endif()

if(BUILD_SERVER AND BUILD_CLIENT)
    add_subdirectory(JointClientKit)
endif()