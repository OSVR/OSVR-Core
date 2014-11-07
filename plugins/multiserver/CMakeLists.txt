add_library(org_opengoggles_bundled_Multiserver MODULE 
    org_opengoggles_bundled_Multiserver.cpp
    GetVRPNConnection.cpp
    GetVRPNConnection.h)
target_link_libraries(org_opengoggles_bundled_Multiserver ogvrPluginKitCpp ogvrConnection ogvrPluginHost vendored-vrpn)
set_target_properties(org_opengoggles_bundled_Multiserver PROPERTIES
    PREFIX ""
    FOLDER "OGVR Plugins")
