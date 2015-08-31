/** @file
    @brief Simple example OpenGL application using the OSVR ClientKit display
   methods. This example shows the use of the C API display methods, even though
   it is in C++. See OpenGLSample.cpp for a more idiomatic-C++ sample using the
   C++ API headers.

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/ClientKit/DisplayC.h>
#include "SDL2Helpers.h"
#include "OpenGLCube.h"

// Library/third-party includes
#include <SDL.h>
#include <SDL_opengl.h>

// Standard includes
#include <iostream>

static auto const WIDTH = 1920;
static auto const HEIGHT = 1080;

// Forward declarations of rendering functions defined below.
void render(OSVR_DisplayConfig disp);
void renderScene();

int main(int argc, char *argv[]) {
    namespace SDL = osvr::SDL2;

    // Open SDL
    SDL::Lib lib;

    // Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create a window
    auto window = SDL::createWindow("OSVR", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create an OpenGL context and make it current.
    SDL::GLContext glctx(window.get());

    // Turn on V-SYNC
    SDL_GL_SetSwapInterval(1);

    // Start OSVR and get OSVR display config
    osvr::clientkit::ClientContext ctx("com.osvr.example.SDLOpenGL");
    OSVR_DisplayConfig display;
    auto ret = osvrClientGetDisplay(ctx.get(), &display);
    if (ret != OSVR_RETURN_SUCCESS) {
        std::cerr << "\nCould not get display config (server probably not "
                     "running or not behaving), exiting."
                  << std::endl;
        return -1;
    }

    std::cout << "Waiting for the display to fully start up, including "
                 "receiving initial pose update..."
              << std::endl;
    while (osvrClientCheckDisplayStartup(display) != OSVR_RETURN_SUCCESS) {
        ctx.update();
    }
    std::cout << "OK, display startup status is good!" << std::endl;

    // Event handler
    SDL_Event e;
#ifndef __ANDROID__ // Don't want to pop up the on-screen keyboard
    SDL::TextInput textinput;
#endif
    bool quit = false;
    while (!quit) {
        // Handle all queued events
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                // Handle some system-wide quit event
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (SDL_SCANCODE_ESCAPE == e.key.keysym.scancode) {
                    // Handle pressing ESC
                    quit = true;
                }
                break;
            }
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Update OSVR
        ctx.update();

        // Render
        render(display);

        // Swap buffers
        SDL_GL_SwapWindow(window.get());
    }

    return 0;
}

/// @brief A simple dummy "draw" function - note that drawing occurs in "room
/// space" by default. (that is, in this example, the modelview matrix when this
/// function is called is initialized such that it transforms from world space
/// to view space)
void renderScene() { draw_cube(1.0); }

/// @brief The "wrapper" for rendering to a device described by OSVR.
///
/// This function will set up viewport, initialize view and projection matrices
/// to current values, then call `renderScene()` as needed (e.g. once for each
/// eye, for a simple HMD.)
void render(OSVR_DisplayConfig disp) {
    /// For each viewer...
    OSVR_ViewerCount viewers;
    osvrClientGetNumViewers(disp, &viewers);

    // Clear the screen to black and clear depth
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {

        /// For each eye of the given viewer...
        OSVR_EyeCount eyes;
        osvrClientGetNumEyesForViewer(disp, viewer, &eyes);
        for (OSVR_EyeCount eye = 0; eye < eyes; ++eye) {

            /// Try retrieving the view matrix (based on eye pose) from OSVR
            double viewMat[OSVR_MATRIX_SIZE];
            osvrClientGetViewerEyeViewMatrixd(
                disp, viewer, eye,
                OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS, viewMat);

            /// Initialize the ModelView transform with the view matrix we
            /// received
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glMultMatrixd(viewMat);

            /// For each display surface seen by the given eye of the given
            /// viewer...
            OSVR_SurfaceCount surfaces;
            osvrClientGetNumSurfacesForViewerEye(disp, viewer, eye, &surfaces);
            for (OSVR_SurfaceCount surface = 0; surface < surfaces; ++surface) {

                /// Set the OpenGL viewport based on the one we computed.
                OSVR_ViewportDimension left;
                OSVR_ViewportDimension bottom;
                OSVR_ViewportDimension width;
                OSVR_ViewportDimension height;
                osvrClientGetRelativeViewportForViewerEyeSurface(
                    disp, viewer, eye, surface, &left, &bottom, &width,
                    &height);

                glViewport(static_cast<GLint>(left), static_cast<GLint>(bottom),
                           static_cast<GLsizei>(width),
                           static_cast<GLsizei>(height));

                /// Set the OpenGL projection matrix based on the one we
                /// computed.
                double zNear = 0.1;
                double zFar = 100;
                double projMat[OSVR_MATRIX_SIZE];
                osvrClientGetViewerEyeSurfaceProjectionMatrixd(
                    disp, viewer, eye, surface, zNear, zFar,
                    OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS |
                        OSVR_MATRIX_SIGNEDZ | OSVR_MATRIX_RHINPUT,
                    projMat);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glMultMatrixd(projMat);

                /// Set the matrix mode to ModelView, so render code doesn't
                /// mess with the projection matrix on accident.
                glMatrixMode(GL_MODELVIEW);

                /// Call out to render our scene.
                renderScene();
            }
        }
    }
    /// Successfully completed a frame render.
}
