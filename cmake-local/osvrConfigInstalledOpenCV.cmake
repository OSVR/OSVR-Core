# File included only with installed binaries, to re-find OpenCV on those systems if needed

if(NOT OPENCV_FOUND)
    if(WIN32)
        # Hints for Windows, given the Chocolatey packages.
        if(NOT "$ENV{ChocolateyInstall}" STREQUAL "")
            file(TO_CMAKE_PATH "$ENV{ChocolateyInstall}/lib" CHOCO_LIB)
            list(APPEND CMAKE_PREFIX_PATH "${CHOCO_LIB}/OpenCV/tools/opencv/build" "${CHOCO_LIB}/OpenCV.2.4.10.20150209/tools/opencv/build")
        endif()
    endif()
    find_package(OpenCV QUIET COMPONENTS core)
endif()

# OpenCV is required for these targets, so adding regardless of if we found it - make the build fail early if it's not found.
set_property(TARGET osvr::osvrPluginKitImaging APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${OpenCV_LIBS})
set_property(TARGET osvr::osvrPluginKitImaging APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OpenCV_INCLUDE_DIRS})
