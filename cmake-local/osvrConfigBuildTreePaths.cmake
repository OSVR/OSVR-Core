# needed for some OSVR libraries
set(libfunctionality_DIR_IMPORTED "@libfunctionality_DIR@")
if(libfunctionality_DIR_IMPORTED AND NOT LIBFUNCTIONALITY_FOUND)
    list(APPEND CMAKE_PREFIX_PATH "${libfunctionality_DIR_IMPORTED}")
endif()

# If we know where jsoncpp is, we'll be polite and share.
set(jsoncpp_DIR_IMPORTED "@jsoncpp_DIR@")
if(jsoncpp_DIR_IMPORTED AND NOT JSONCPP_FOUND)
    list(APPEND CMAKE_PREFIX_PATH "${jsoncpp_DIR_IMPORTED}")
endif()

# Likewise for OpenCV.
set(OpenCV_DIR_IMPORTED "@OpenCV_DIR@")
if(OpenCV_DIR_IMPORTED AND NOT OPENCV_FOUND)
    list(APPEND CMAKE_PREFIX_PATH "${OpenCV_DIR_IMPORTED}")
endif()

# Likewise for our vendored header-only libraries - just Eigen for now.
if(NOT EIGEN3_FOUND)
    list(APPEND CMAKE_PREFIX_PATH "@OSVR_VENDORED_EIGEN_ROOT@")
endif()

# Help out with boost.
if(NOT BOOST_FOUND)
    include("${CMAKE_CURRENT_LIST_DIR}/CMakeBoostHelper.cmake" OPTIONAL)
endif()

# For build trees, we might already know where an OSVR JSON to C is
if(NOT OSVR_JSON_TO_C_COMMAND)
    set(OSVR_JSON_TO_C_COMMAND "@OSVR_JSON_TO_C_COMMAND@")
endif()
