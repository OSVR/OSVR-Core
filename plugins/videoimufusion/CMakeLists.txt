
osvr_convert_json(org_osvr_filter_videoimufusion_json
    org_osvr_filter_videoimufusion.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

osvr_add_plugin(NAME org_osvr_filter_videoimufusion
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    org_osvr_filter_videoimufusion.cpp
    VideoIMUFusion.h
    VideoIMUFusion.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h")

target_link_libraries(org_osvr_filter_videoimufusion
    osvr::osvrAnalysisPluginKit
    eigen-headers
    jsoncpp_lib)

target_compile_options(org_osvr_filter_videoimufusion
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(org_osvr_filter_videoimufusion PROPERTIES
    FOLDER "OSVR Plugins")
