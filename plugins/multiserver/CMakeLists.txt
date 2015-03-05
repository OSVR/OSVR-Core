osvr_add_plugin(NAME org_opengoggles_bundled_Multiserver
    CPP
    SOURCES
    org_opengoggles_bundled_Multiserver.cpp
    DevicesWithParameters.cpp
    DevicesWithParameters.h
    GetSerialPortState.cpp
    GetSerialPortState.h
    VRPNMultiserver.cpp
    VRPNMultiserver.h)

target_link_libraries(org_opengoggles_bundled_Multiserver osvrVRPNServer osvrConnection osvrPluginHost jsoncpp_lib vendored-vrpn)

target_include_directories(org_opengoggles_bundled_Multiserver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi")

set_target_properties(org_opengoggles_bundled_Multiserver PROPERTIES
    FOLDER "OSVR Plugins")

install(FILES
    org_opengoggles_bundled_Multiserver_RazerHydra.json
    org_opengoggles_bundled_Multiserver_YEI_3Space_Sensor.json
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)
