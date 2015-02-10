
add_library(com_osvr_VideoCapture_OpenCV MODULE com_osvr_VideoCapture_OpenCV.cpp)

#osvr_convert_json(com_osvr_VideoCapture_OpenCV_json com_osvr_VideoCapture_OpenCV.json "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoCapture_OpenCV_json.h")

target_link_libraries(com_osvr_VideoCapture_OpenCV osvrPluginKitCpp ${OpenCV_LIBRARIES})

set_target_properties(com_osvr_VideoCapture_OpenCV PROPERTIES
    PREFIX ""
    FOLDER "OSVR Plugins")

install(TARGETS com_osvr_VideoCapture_OpenCV
    LIBRARY DESTINATION ${OSVR_PLUGIN_DIR} COMPONENT Runtime)

