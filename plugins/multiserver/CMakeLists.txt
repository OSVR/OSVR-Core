set(DESCRIPTORS
    com_osvr_Multiserver_OneEuroFilter
    com_osvr_Multiserver_OSVRHackerDevKit
    com_osvr_Multiserver_RazerHydra
    com_osvr_Multiserver_YEI_3Space_Sensor
    com_osvr_Multiserver_Sensics_zSight)
set(JSON_HEADERS)
set(JSON_DESCRIPTORS)

foreach(filestem ${DESCRIPTORS})
    set(infile ${filestem}.json)
    set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${filestem}_json.h")
    osvr_convert_json(${filestem}_json
        ${infile}
        "${outfile}")
    list(APPEND JSON_HEADERS "${outfile}")
    list(APPEND JSON_DESCRIPTORS "${infile}")
endforeach()

if(BUILD_USBSERIALENUM)
    add_definitions(-DOSVR_HAVE_USBSERIALENUM)
endif()

osvr_add_plugin(NAME com_osvr_Multiserver
    CPP
    SOURCES
    com_osvr_Multiserver.cpp
    DevicesWithParameters.cpp
    DevicesWithParameters.h
    GetSerialPortState.cpp
    GetSerialPortState.h
    VRPNMultiserver.cpp
    VRPNMultiserver.h
    ${JSON_HEADERS})

target_link_libraries(com_osvr_Multiserver osvrVRPNServer osvrConnection osvrPluginHost JsonCpp::JsonCpp vendored-vrpn vendored-hidapi)

if(BUILD_USBSERIALENUM)
    target_link_libraries(com_osvr_Multiserver osvrUSBSerial)
endif()

target_include_directories(com_osvr_Multiserver PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

set_target_properties(com_osvr_Multiserver PROPERTIES
    FOLDER "OSVR Plugins")

install(FILES
    ${JSON_DESCRIPTORS}
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)
