
osvr_convert_json(org_osvr_filter_oneeuro_json
    org_osvr_filter_oneeuro.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_oneeuro_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

osvr_add_plugin(NAME org_osvr_filter_oneeuro
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    org_osvr_filter_oneeuro.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_oneeuro_json.h")

target_link_libraries(org_osvr_filter_oneeuro
    osvr::osvrAnalysisPluginKit
    eigen-headers
    JsonCpp::JsonCpp)

target_compile_options(org_osvr_filter_oneeuro
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(org_osvr_filter_oneeuro PROPERTIES
    FOLDER "OSVR Plugins")
