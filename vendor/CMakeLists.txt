if(BUILD_TESTING)
    if(WIN32)
        set(gtest_force_shared_crt ON)
    endif()
    add_subdirectory(gtest-1.7.0)
endif()
set(BUILD_SHARED_LIBS OFF)

if(NOT (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vrpn/vrpn_Connection.h" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vrpn/submodules/hidapi/hidapi/hidapi.h"))
	message(FATAL_ERROR "You are missing the VRPN git submodule or one of its submodules- you must have not done a recursive clone. You can get it now by running git submodule update --init --recursive from the root source directory.")
endif()

# Build VRPN as subproject
set(VRPN_GPL_SERVER FALSE CACHE INTERNAL "" FORCE)
if(BUILD_CLIENT AND NOT BUILD_SERVER)
    set(VRPN_BUILD_SERVER_LIBRARY FALSE CACHE INTERNAL "" FORCE)
    set(VRPN_BUILD_CLIENT_LIBRARY TRUE CACHE INTERNAL "" FORCE)
else()
    set(VRPN_BUILD_SERVER_LIBRARY TRUE CACHE INTERNAL "" FORCE)
    set(VRPN_BUILD_CLIENT_LIBRARY FALSE CACHE INTERNAL "" FORCE)
    set(VRPN_USE_HID TRUE CACHE INTERNAL "" FORCE)
    set(VRPN_USE_LOCAL_HIDAPI TRUE CACHE INTERNAL "" FORCE)
    if(NOT WIN32)
        set(VRPN_USE_LIBUSB_1_0 TRUE CACHE INTERNAL "" FORCE)
    endif()
endif()
add_subdirectory(vrpn)

# Create an interface target to more easily consume VRPN internally.
add_library(vendored-vrpn INTERFACE)
if(BUILD_CLIENT AND NOT BUILD_SERVER)
    target_link_libraries(vendored-vrpn INTERFACE ${VRPN_CLIENT_LIBRARY})
else()
    target_link_libraries(vendored-vrpn INTERFACE ${VRPN_SERVER_LIBRARY})
endif()
target_include_directories(vendored-vrpn INTERFACE ${VRPN_INCLUDE_DIRS})
target_compile_definitions(vendored-vrpn INTERFACE ${VRPN_DEFINITIONS})

# Interface target for hidapi compiled as part of VRPN.
add_library(vendored-hidapi INTERFACE)
target_link_libraries(vendored-hidapi INTERFACE vendored-vrpn)
if(VRPN_HIDAPI_SOURCE_ROOT)
    target_include_directories(vendored-hidapi INTERFACE "${VRPN_HIDAPI_SOURCE_ROOT}")
else()
    target_include_directories(vendored-hidapi INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vrpn/submodules/hidapi")
endif()

# Interface target for util headers
add_library(util-headers INTERFACE)
target_include_directories(util-headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/util-headers")

# Interface target for the run loop manager in util headers
add_library(util-runloopmanager INTERFACE)
target_include_directories(util-runloopmanager INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/util-headers")
target_link_libraries(util-runloopmanager INTERFACE boost_thread)

# Interface target for Eigen
add_library(eigen-headers INTERFACE)
target_include_directories(eigen-headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/eigen-3.2.4")
target_compile_definitions(eigen-headers INTERFACE EIGEN_MPL2_ONLY)
