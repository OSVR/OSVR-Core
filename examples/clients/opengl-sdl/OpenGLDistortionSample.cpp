/** @file
    @brief Implementation

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
#include <osvr/ClientKit/Display.h>

#include "vert.h"
#include "frag.h"

// Library/third-party includes
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "SDL2Helpers.h"

// Standard includes
#include <iostream>

static auto const WIDTH = 1920;
static auto const HEIGHT = 1080;

// Forward declarations of rendering functions defined below.
void draw_cube(double radius);
bool render(osvr::clientkit::DisplayConfig &disp);
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
    osvr::clientkit::DisplayConfig display(ctx);
    if (!display.valid()) {
        std::cerr << "\nCould not get display config (server probably not "
            "running or not behaving), exiting."
            << std::endl;
        return -1;
    }

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
        bool valid = render(display);

        // Swap buffers if we had all the data to render.
        if (valid) {
            SDL_GL_SwapWindow(window.get());
        }
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
///
/// @return false if we didn't render (typically because we just started up and
/// don't have pose data for head/eyes yet)
bool render(osvr::clientkit::DisplayConfig &disp) {
    static bool announcedPose = false;
    bool success = false;

    // Clear the screen to black and clear depth
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// For each viewer, eye combination...
    disp.forEachEye([&success](osvr::clientkit::Eye eye) {

        /// Try retrieving the view matrix (based on eye pose) from OSVR
        double viewMat[OSVR_MATRIX_SIZE];
        auto gotView = eye.getViewMatrix(
            OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS, viewMat);
        if (!gotView) {
            std::cout << "Waiting for view pose..." << std::endl;
            return;
        }
        if (!announcedPose) {
            std::cout << "Got view pose, rendering will commence!" << std::endl;
            announcedPose = true;
        }
        /// Initialize the ModelView transform with the view matrix we
        /// received
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMultMatrixd(viewMat);

        /// For each display surface seen by the given eye of the given
        /// viewer...
        eye.forEachSurface([](osvr::clientkit::Surface surface) {
            auto viewport = surface.getRelativeViewport();
            glViewport(static_cast<GLint>(viewport.left),
                static_cast<GLint>(viewport.bottom),
                static_cast<GLsizei>(viewport.width),
                static_cast<GLsizei>(viewport.height));

            /// Set the OpenGL projection matrix based on the one we
            /// computed.
            double zNear = 0.1;
            double zFar = 100;
            double projMat[OSVR_MATRIX_SIZE];
            surface.getProjectionMatrix(
                zNear, zFar, OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS |
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
        });
        success = true;
    });

    /// Successfully completed a frame render.
    return success;
}

/// @brief Fixed-function pipeline OpenGL code to draw a cube
void draw_cube(double radius) {
    static const GLfloat matspec[4] = { 0.5, 0.5, 0.5, 0.0 };
    static const float red_col[] = { 1.0, 0.0, 0.0 };
    static const float grn_col[] = { 0.0, 1.0, 0.0 };
    static const float blu_col[] = { 0.0, 0.0, 1.0 };
    static const float yel_col[] = { 1.0, 1.0, 0.0 };
    static const float lightblu_col[] = { 0.0, 1.0, 1.0 };
    static const float pur_col[] = { 1.0, 0.0, 1.0 };
    glPushMatrix();
    glScaled(radius, radius, radius);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matspec);
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
    glBegin(GL_POLYGON);
    glColor3fv(lightblu_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lightblu_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightblu_col);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3fv(blu_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blu_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blu_col);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3fv(yel_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, yel_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yel_col);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3fv(grn_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grn_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grn_col);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3fv(pur_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pur_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pur_col);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3fv(red_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red_col);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red_col);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glEnd();
    glPopMatrix();
}
