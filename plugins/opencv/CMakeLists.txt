#osvr_convert_json(com_osvr_VideoCapture_OpenCV_json com_osvr_VideoCapture_OpenCV.json "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoCapture_OpenCV_json.h")
osvr_add_plugin(NAME com_osvr_VideoCapture_OpenCV
    MANUAL_LOAD
    SOURCES com_osvr_VideoCapture_OpenCV.cpp)

target_link_libraries(com_osvr_VideoCapture_OpenCV osvrPluginKitImaging opencv_core opencv_highgui opencv_imgproc)

set_target_properties(com_osvr_VideoCapture_OpenCV PROPERTIES
    FOLDER "OSVR Plugins")
