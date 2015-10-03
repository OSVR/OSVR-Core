
if(NOT OPENGL_FOUND OR NOT SDL2_FOUND)
    return()
endif()

set(SHARED_FILES SDL2Helpers.h OpenGLCube.h)
add_executable(OpenGLSample ${SHARED_FILES} OpenGLSample.cpp)
target_include_directories(OpenGLSample
    PRIVATE
    ${SDL2_INCLUDE_DIR})

target_link_libraries(OpenGLSample
    PRIVATE
    osvrClientKitCpp
    ${OPENGL_LIBRARY}
    ${SDL2_LIBRARY}
    osvr_cxx11_flags)
if(SDL2_DYNAMIC)
    osvr_copy_dep(OpenGLSample ${SDL2_LIBRARY})
endif()

add_executable(OpenGLSampleCAPI ${SHARED_FILES} OpenGLSampleCAPI.cpp)
target_include_directories(OpenGLSampleCAPI
    PRIVATE
    ${SDL2_INCLUDE_DIR})
target_link_libraries(OpenGLSampleCAPI
    PRIVATE
    osvrClientKitCpp
    ${OPENGL_LIBRARY}
    ${SDL2_LIBRARY}
    osvr_cxx11_flags)

if(BUILD_JOINTCLIENTKIT)
    set(OPENGL_JOINT_SAMPLE OpenGLSampleJoint)
    add_executable(OpenGLSampleJoint ${SHARED_FILES} OpenGLSampleJoint.cpp)
    target_include_directories(OpenGLSampleJoint
        PRIVATE
        ${SDL2_INCLUDE_DIR})
    target_link_libraries(OpenGLSampleJoint
        PRIVATE
        osvrJointClientKit
        osvrClientKitCpp
        ${OPENGL_LIBRARY}
        ${SDL2_LIBRARY}
        osvr_cxx11_flags)
endif()

foreach(target OpenGLSample OpenGLSampleCAPI ${OPENGL_JOINT_SAMPLE})
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR Example Clients")

    install(TARGETS ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endforeach()
