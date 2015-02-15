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