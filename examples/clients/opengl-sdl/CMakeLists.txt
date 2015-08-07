
if(NOT OPENGL_FOUND OR NOT SDL2_FOUND)
    return()
endif()

add_executable(OpenGLSample OpenGLSample.cpp)
message(STATUS "SDL2_LIBRARY ${SDL2_LIBRARY}")
target_link_libraries(OpenGLSample PRIVATE osvrClientKitCpp ${OPENGL_LIBRARY} SDL2::SDL2main)
set_target_properties(OpenGLSample PROPERTIES
    FOLDER "OSVR Example Clients")
