
if(NOT OPENGL_FOUND OR NOT SDL2_FOUND)
    return()
endif()

add_executable(OpenGLSample SDL2Helpers.h OpenGLSample.cpp)
target_link_libraries(OpenGLSample
    PRIVATE
    osvrClientKitCpp
    ${OPENGL_LIBRARY}
    SDL2::SDL2main
    osvr_cxx11_flags)

if(SDL2_DYNAMIC)
    osvr_copy_dep(OpenGLSample SDL2::SDL2)
endif()

add_executable(OpenGLSampleCAPI SDL2Helpers.h OpenGLSampleCAPI.cpp)
target_link_libraries(OpenGLSampleCAPI
    PRIVATE
    osvrClientKitCpp
    ${OPENGL_LIBRARY}
    SDL2::SDL2main
    osvr_cxx11_flags)

foreach(target OpenGLSample OpenGLSampleCAPI)
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR Example Clients")

    install(TARGETS ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endforeach()

