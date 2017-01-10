osvr_convert_json(com_Sensics_Tracker_Spin_json
    com_Sensics_Tracker_Spin.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_Sensics_Tracker_Spin_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Build and install the plugin
osvr_add_plugin(com_Sensics_Tracker_Spin
                SOURCES
                com_Sensics_Tracker_Spin.cpp
                "${CMAKE_CURRENT_BINARY_DIR}/com_Sensics_Tracker_Spin_json.h")

target_link_libraries(com_Sensics_Tracker_Spin
                      osvrPluginKitCpp
                      osvrUtilCpp
                      JsonCpp::JsonCpp
                      vendored-vrpn)

target_compile_options(com_Sensics_Tracker_Spin
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(com_Sensics_Tracker_Spin PROPERTIES
    FOLDER "OSVR Plugins")