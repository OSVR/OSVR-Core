
osvr_convert_json(org_osvr_filter_deadreckoningrotation_json
    org_osvr_filter_deadreckoningrotation.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_deadreckoningrotation_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

osvr_add_plugin(NAME org_osvr_filter_deadreckoningrotation
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    org_osvr_filter_deadreckoningrotation.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_deadreckoningrotation_json.h")

target_link_libraries(org_osvr_filter_deadreckoningrotation
    osvr::osvrAnalysisPluginKit
    osvrVRPNServer
    vendored-vrpn
    JsonCpp::JsonCpp)

target_compile_options(org_osvr_filter_deadreckoningrotation
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(org_osvr_filter_deadreckoningrotation PROPERTIES
    FOLDER "OSVR Plugins")
