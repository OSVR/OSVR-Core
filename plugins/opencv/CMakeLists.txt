include_directories("${CMAKE_CURRENT_BINARY_DIR}")
osvr_convert_json(com_osvr_VideoCapture_OpenCV_json com_osvr_VideoCapture_OpenCV.json "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoCapture_OpenCV_json.h")
osvr_add_plugin(NAME com_osvr_VideoCapture_OpenCV
    MANUAL_LOAD
    SOURCES
    com_osvr_VideoCapture_OpenCV.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoCapture_OpenCV_json.h")

target_link_libraries(com_osvr_VideoCapture_OpenCV osvrPluginKitImaging opencv_core ${OPENCV_CAMERA_EXTRA_LIBS} osvr_cxx11_flags)
target_include_directories(com_osvr_VideoCapture_OpenCV PRIVATE ${OpenCV_INCLUDE_DIRS})

set_target_properties(com_osvr_VideoCapture_OpenCV PROPERTIES
    FOLDER "OSVR Plugins")
