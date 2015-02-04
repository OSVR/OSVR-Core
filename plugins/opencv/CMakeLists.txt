
find_package(OpenCV)

if (OPENCV_FOUND)
    add_library(com_osvr_OpenCV MODULE com_osvr_OpenCV.cpp)

    #osvr_convert_json(com_osvr_OpenCV_json com_osvr_OpenCV.json "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_OpenCV_json.h")

    target_link_libraries(com_osvr_OpenCV osvrPluginKitCpp ${OPENCV_LIBRARIES})
    target_include_directories(com_osvr_OpenCV PRIVATE ${OPENCV_INCLUDE_DIRS})

    set_target_properties(com_osvr_OpenCV PROPERTIES
        PREFIX ""
        FOLDER "OSVR Plugins")

    install(TARGETS com_osvr_OpenCV
        LIBRARY DESTINATION ${OSVR_PLUGIN_DIR} COMPONENT Runtime)
endif()

