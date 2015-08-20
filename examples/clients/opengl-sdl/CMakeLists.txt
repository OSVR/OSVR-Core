
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


if(GLEW_FOUND)
    osvr_embed_shader(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/K1RadialDistort.frag" DEST "${CMAKE_CURRENT_BINARY_DIR}/frag.h" SYMBOL FRAG_STRING)
    osvr_embed_shader(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/K1RadialDistort.vert" DEST "${CMAKE_CURRENT_BINARY_DIR}/vert.h" SYMBOL VERT_STRING)
    add_executable(OpenGLDistortionSample
        "${CMAKE_CURRENT_BINARY_DIR}/frag.h"
        "${CMAKE_CURRENT_BINARY_DIR}/vert.h"
        SDL2Helpers.h
        OpenGLDistortionSample.cpp)
    target_include_directories(OpenGLDistortionSample PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(OpenGLDistortionSample
        PRIVATE
        osvrClientKitCpp
        ${OPENGL_LIBRARY}
        SDL2::SDL2main
        GLEW::GLEW
        osvr_cxx11_flags)
    set_target_properties(OpenGLDistortionSample PROPERTIES
        FOLDER "OSVR Example Clients")

    if(GLEW_LIBRARY MATCHES ".*s.lib")
        target_compile_definitions(OpenGLDistortionSample PRIVATE GLEW_STATIC)
    endif()
    install(TARGETS OpenGLDistortionSample
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endif()
