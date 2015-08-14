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
#include <osvr/ClientKit/DisplayC.h>

// Library/third-party includes
#include <SDL.h>
#include <SDL_opengl.h>

// Standard includes
#include <iostream>
#include <stdexcept>
#include <memory>

using std::cout;
using std::cerr;
using std::endl;

namespace SDL {
/// @brief RAII wrapper for SDL startup/shutdown
class Lib {
  public:
    Lib() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cerr << "Could not initialize SDL" << endl;
            throw std::runtime_error(std::string("Could not initialize SDL") +
                                     SDL_GetError());
        }
    }
    ~Lib() { SDL_Quit(); }

    Lib(Lib const &) = delete;            //< non-copyable
    Lib &operator=(Lib const &) = delete; //< non-assignable
};

/// @brief Smart pointer for SDL_Window
typedef std::shared_ptr<SDL_Window> WindowPtr;

/// @brief Smart pointer constructor for SDL_Window (forwarding)
template <typename... Args> inline WindowPtr createWindow(Args &&... args) {
    WindowPtr ret(SDL_CreateWindow(std::forward<Args>(args)...),
                  &SDL_DestroyWindow);
    return ret;
}

/// @brief RAII wrapper for SDL text input start/stop.
class TextInput {
  public:
    TextInput() { SDL_StartTextInput(); }
    ~TextInput() { SDL_StopTextInput(); }

    TextInput(TextInput const &) = delete;            //< non-copyable
    TextInput &operator=(TextInput const &) = delete; //< non-assignable
};

/// @brief RAII wrapper for SDL OpenGL context.
class GLContext {
  public:
    /// @brief Forwarding constructor
    template <typename... Args> GLContext(Args &&... args) {
        m_context = SDL_GL_CreateContext(std::forward<Args>(args)...);
    }

    /// @brief Destructor for cleanup
    ~GLContext() { SDL_GL_DeleteContext(m_context); }

    /// @brief Implicit conversion
    operator SDL_GLContext() const { return m_context; }

    GLContext(GLContext const &) = delete;            //< non-copyable
    GLContext &operator=(GLContext const &) = delete; //< non-assignable

  private:
    SDL_GLContext m_context;
};

} // namespace SDL

static auto const WIDTH = 1920;
static auto const HEIGHT = 1080;

// Annoyingly a preprocessor macro is the only "good" way to do this quickly and
// easily
#define CHECK_SDL_POINTER_RESULT(ptr, action)                                  \
    do {                                                                       \
        if (!ptr) {                                                            \
            cerr << "Could not " action ": " << SDL_GetError() << endl;        \
            std::exit(-1);                                                     \
        }                                                                      \
    } while (0)
#define CHECK_SDL_NEGATIVE_RESULT(op, action)                                  \
    do {                                                                       \
        if ((op) < 0) {                                                        \
            cerr << "Could not " action ": " << SDL_GetError() << endl;        \
            std::exit(-1);                                                     \
        }                                                                      \
    } while (0)

// Forward declarations of rendering functions defined below.
void draw_cube(double radius);
bool render(OSVR_DisplayConfig disp);
void renderScene();

int main(int argc, char *argv[]) {
    // Open SDL
    SDL::Lib lib;

    // Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Create a window
    auto window = SDL::createWindow("OSVR", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    CHECK_SDL_POINTER_RESULT(window, "create window");

    // Create an OpenGL context and make it current.
    SDL::GLContext glctx(window.get());

    // Turn on V-SYNC
    SDL_GL_SetSwapInterval(1);

    // Start OSVR and get OSVR display config
    osvr::clientkit::ClientContext ctx("com.osvr.example.sdlopengl");
    OSVR_DisplayConfig display;
    auto ret = osvrClientGetDisplay(ctx.get(), &display);
    if (ret != OSVR_RETURN_SUCCESS) {
        std::cerr << "\nCould not get display config (server probably not "
                     "running or not behaving), exiting." << std::endl;
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
bool render(OSVR_DisplayConfig disp) {
    /// For each viewer...
    OSVR_ViewerCount viewers;
    osvrClientGetNumViewers(disp, &viewers);
    for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {

        /// For each eye of the given viewer...
        OSVR_EyeCount eyes;
        osvrClientGetNumEyesForViewer(disp, viewer, &eyes);
        for (OSVR_EyeCount eye = 0; eye < eyes; ++eye) {

            /// Try retrieving the view matrix (based on eye pose) from OSVR
            double viewMat[OSVR_MATRIX_SIZE];
            auto gotView = osvrClientGetViewerEyeViewMatrixd(
                disp, viewer, eye, viewMat,
                OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS);
            if (gotView != OSVR_RETURN_SUCCESS) {
                cout << "Waiting for view pose..." << endl;
                return false;
            }

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
                double projMat[16];
                osvrClientGetProjectionMatrixdForViewerEyeSurface(
                    disp, viewer, eye, surface, zNear, zFar, projMat,
                    OSVR_MATRIX_COLMAJOR | OSVR_MATRIX_COLVECTORS |
                        OSVR_MATRIX_SIGNEDZ | OSVR_MATRIX_RHINPUT);
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
    return true;
}

/// @brief Fixed-function pipeline OpenGL code to draw a cube
void draw_cube(double radius) {
    static const GLfloat matspec[4] = {0.5, 0.5, 0.5, 0.0};
    static const float red_col[] = {1.0, 0.0, 0.0};
    static const float grn_col[] = {0.0, 1.0, 0.0};
    static const float blu_col[] = {0.0, 0.0, 1.0};
    static const float yel_col[] = {1.0, 1.0, 0.0};
    static const float lightblu_col[] = {0.0, 1.0, 1.0};
    static const float pur_col[] = {1.0, 0.0, 1.0};
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
