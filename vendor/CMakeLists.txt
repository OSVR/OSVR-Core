if(BUILD_TESTING)
    if(WIN32)
        set(gtest_force_shared_crt ON)
    endif()
    add_subdirectory(gtest-1.7.0)
endif()
set(BUILD_SHARED_LIBS OFF)

# Build VRPN as subproject
set(VRPN_GPL_SERVER FALSE CACHE INTERNAL "" FORCE)
set(VRPN_BUILD_SERVER_LIBRARY TRUE CACHE INTERNAL "" FORCE)
set(VRPN_BUILD_CLIENT_LIBRARY TRUE CACHE INTERNAL "" FORCE)
set(VRPN_USE_HID TRUE CACHE INTERNAL "" FORCE)
set(VRPN_USE_LOCAL_HIDAPI TRUE CACHE INTERNAL "" FORCE)
if(NOT WIN32)
    set(VRPN_USE_LIBUSB_1_0 TRUE CACHE INTERNAL "" FORCE)
endif()
add_subdirectory(vrpn)

# Create an interface target to more easily consume VRPN internally.
add_library(vendored-vrpn INTERFACE)
target_link_libraries(vendored-vrpn INTERFACE ${VRPN_SERVER_LIBRARY})
target_include_directories(vendored-vrpn INTERFACE ${VRPN_INCLUDE_DIRS})
target_compile_definitions(vendored-vrpn INTERFACE ${VRPN_DEFINITIONS})

# Interface target for hidapi compiled as part of VRPN.
add_library(hidapi INTERFACE)
target_link_libraries(hidapi INTERFACE vendored-vrpn)
target_include_directories(hidapi INTERFACE vrpn/submodules/hidapi)

# Interface target for util headers
add_library(util-headers INTERFACE)
target_include_directories(util-headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/util-headers")

# Interface target for the run loop manager in util headers
add_library(util-runloopmanager INTERFACE)
target_include_directories(util-runloopmanager INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/util-headers" ${Boost_INCLUDE_DIRS})
target_link_libraries(util-runloopmanager INTERFACE
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})

# Interface target for QVRPN headers
add_library(qvrpn-headers INTERFACE)
target_include_directories(qvrpn-headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/qvrpn")

# Interface target for Eigen 3.2.2
add_library(eigen-headers INTERFACE)
target_include_directories(eigen-headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/eigen-3.2.2")
target_compile_definitions(eigen-headers INTERFACE EIGEN_MPL2_ONLY)