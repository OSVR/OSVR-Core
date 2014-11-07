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
add_subdirectory(vrpn)

# Create an interface target to more easily consume VRPN internally.
add_library(vendored-vrpn INTERFACE)
target_link_libraries(vendored-vrpn INTERFACE ${VRPN_SERVER_LIBRARY})
target_include_directories(vendored-vrpn INTERFACE ${VRPN_INCLUDE_DIRS})
target_compile_definitions(vendored-vrpn INTERFACE ${VRPN_DEFINITIONS})