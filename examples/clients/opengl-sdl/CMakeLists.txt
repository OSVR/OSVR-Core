
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
set_target_properties(OpenGLSample PROPERTIES
    FOLDER "OSVR Example Clients")

install(TARGETS OpenGLSample
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)

if(SDL2_DYNAMIC)
    osvr_copy_dep(OpenGLSample SDL2::SDL2)
endif()
