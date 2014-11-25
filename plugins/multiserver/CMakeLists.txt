add_library(org_opengoggles_bundled_Multiserver MODULE
    org_opengoggles_bundled_Multiserver.cpp
    DevicesWithParameters.cpp
    DevicesWithParameters.h
    GetVRPNConnection.cpp
    GetVRPNConnection.h
    VRPNMultiserver.cpp
    VRPNMultiserver.h)
target_link_libraries(org_opengoggles_bundled_Multiserver osvrPluginKitCpp osvrConnection osvrPluginHost jsoncpp_lib vendored-vrpn)
target_include_directories(org_opengoggles_bundled_Multiserver PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi")
set_target_properties(org_opengoggles_bundled_Multiserver PROPERTIES
    PREFIX ""
    FOLDER "OSVR Plugins")

install(TARGETS org_opengoggles_bundled_Multiserver
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)