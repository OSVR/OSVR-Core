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
            throw std::runtime_error("Could not initialize SDL");
        }
    }
    ~Lib() { SDL_Quit(); }
};

typedef std::shared_ptr<SDL_Window> WindowPtr;
template <typename... Args> inline WindowPtr createWindow(Args &&... args) {
    WindowPtr ret(SDL_CreateWindow(std::forward<Args>(args)...),
                  &SDL_DestroyWindow);
    return ret;
}
} // namespace
int main(int argc, char *argv[]) {
    SDL::Lib lib;

    // Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    auto window = SDL::createWindow("OSVR", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, 1920, 1080,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Could not create window" << endl;
        return -1;
    }


    return 0;
}
