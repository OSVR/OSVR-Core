if(BUILD_DEV_VERBOSE)
    set(OGVR_UTIL_DEV_VERBOSE TRUE)
endif()
configure_file(Verbosity.h.in "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h")

# This header is internal-only.
# install(
    # FILES
    # "${CMAKE_CURRENT_BINARY_DIR}/Verbosity.h"
    # DESTINATION
    # ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/Util
    # COMPONENT
    # Devel)

