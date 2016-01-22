set(VIDEOTRACKER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../videobasedtracker")
set(PROJ_FOLDER "OSVR Plugins/Unified Video-Inertial Tracker")
osvr_convert_json(org_osvr_unifiedvideoinertial_json
    org_osvr_unifiedvideoinertial.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_unifiedvideoinertial_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Make sure we can link these upcoming static libraries into a shared lib
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(ImageSources)

add_executable(uvbi-view-camera ViewTrackingCamera.cpp)
target_link_libraries(uvbi-view-camera PRIVATE uvbi-image-sources)
