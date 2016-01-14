if(WIN32)
    option(OSVR_FPE
        "Enable floating-point exceptions - only works with MSVC and should never be used for shipping code!"
        OFF)
    mark_as_advanced(OSVR_FPE)
endif()

osvr_convert_json(org_osvr_filter_videoimufusion_json
    org_osvr_filter_videoimufusion.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
set(FUSION_COMMON_SOURCES
    FusionParams.h
    RunningData.h
    RunningData.cpp
    RunningDataPredictCorrect.cpp
    VideoIMUFusion.h
    VideoIMUFusion.cpp)
osvr_add_plugin(NAME org_osvr_filter_videoimufusion
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    ${FUSION_COMMON_SOURCES}
    org_osvr_filter_videoimufusion.cpp
    VideoIMUFusionDevice.h
    VideoIMUFusionDevice.cpp
    WrapCallback.h
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h")

target_link_libraries(org_osvr_filter_videoimufusion
    osvr::osvrAnalysisPluginKit
    eigen-headers
    JsonCpp::JsonCpp
    osvrKalman)

target_compile_options(org_osvr_filter_videoimufusion
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(org_osvr_filter_videoimufusion PROPERTIES
    FOLDER "OSVR Plugins")
if(BUILD_TESTING)
    add_executable(VideoIMUFusion_Offline
        ${FUSION_COMMON_SOURCES}
        OfflineFusion.cpp)

    target_link_libraries(VideoIMUFusion_Offline
        osvrCommon
        eigen-headers
        JsonCpp::JsonCpp
        osvrKalman)

    target_compile_options(VideoIMUFusion_Offline
        PRIVATE
        ${OSVR_CXX11_FLAGS})
    target_compile_definitions(VideoIMUFusion_Offline PRIVATE OSVR_VIDEOIMUFUSION_VERBOSE)
    set_target_properties(VideoIMUFusion_Offline PROPERTIES
        FOLDER "OSVR Plugins")
    add_test(NAME VideoIMUFusion_Offline_NearOnDesk
        COMMAND VideoIMUFusion_Offline "${CMAKE_CURRENT_SOURCE_DIR}/near-on-desk.json")
endif()

if(WIN32 AND OSVR_FPE)
    set(msg_type WARNING)
    if(OSVR_OFFICIAL)
        # set in our CI
        set(msg_type FATAL_ERROR)
    endif()
    message(${msg_type} "OSVR_FPE enabled, will affect the VideoIMUFusion plugin! Do not ship these binaries!")

    target_compile_definitions(org_osvr_filter_videoimufusion PRIVATE OSVR_FPE)
    target_link_libraries(org_osvr_filter_videoimufusion FloatExceptions)
    if(TARGET VideoIMUFusion_Offline)
        target_compile_definitions(VideoIMUFusion_Offline PRIVATE OSVR_FPE)
        target_link_libraries(VideoIMUFusion_Offline FloatExceptions)
    endif()
endif()
