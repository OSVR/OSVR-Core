set(DESCRIPTORS
    org_opengoggles_bundled_Multiserver_RazerHydra
    org_opengoggles_bundled_Multiserver_YEI_3Space_Sensor)
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

osvr_add_plugin(NAME org_opengoggles_bundled_Multiserver
    CPP
    SOURCES
    org_opengoggles_bundled_Multiserver.cpp
    DevicesWithParameters.cpp
    DevicesWithParameters.h
    GetSerialPortState.cpp
    GetSerialPortState.h
    VRPNMultiserver.cpp
    VRPNMultiserver.h
    ${JSON_HEADERS})

target_link_libraries(org_opengoggles_bundled_Multiserver osvrVRPNServer osvrConnection osvrPluginHost jsoncpp_lib vendored-vrpn)

target_include_directories(org_opengoggles_bundled_Multiserver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi" "${CMAKE_CURRENT_BINARY_DIR}")

set_target_properties(org_opengoggles_bundled_Multiserver PROPERTIES
    FOLDER "OSVR Plugins")

install(FILES
    ${JSON_DESCRIPTORS}
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)
