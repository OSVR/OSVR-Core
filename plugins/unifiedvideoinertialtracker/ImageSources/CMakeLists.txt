###
# Build a separate static library for image acquisition
###
set(SOURCES
    CVImageSource.cpp
    DK2ImageSource.cpp
    ImageSource.cpp
    ImageSource.h
    ImageSourceFactories.h
    FakeImageSource.cpp
    Oculus_DK2.cpp
    Oculus_DK2.h)
if(WIN32)
    list(APPEND SOURCES
        DirectShowHDKCameraFactory.h
        DirectShowImageSource.cpp
        DirectShowToCV.h)
endif()
add_library(uvbi-image-sources STATIC ${SOURCES})
target_compile_options(uvbi-image-sources
    PUBLIC
    ${OSVR_CXX11_FLAGS})

set_target_properties(uvbi-image-sources PROPERTIES
    FOLDER "${PROJ_FOLDER}")

#target_include_directories(uvbi-image-sources INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(uvbi-image-sources PUBLIC
    opencv_core
    opencv_highgui
    ${VIDEOTRACKER_EXTRA_LIBS}
    PRIVATE
    vendored-vrpn)
if(WIN32)
    target_link_libraries(uvbi-image-sources PUBLIC directshow-camera)
endif()
