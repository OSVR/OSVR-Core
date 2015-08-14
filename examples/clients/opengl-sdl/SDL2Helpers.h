/** @file
    @brief Header containing some helper code for using SDL2 in a C++11
   environment.

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

#ifndef INCLUDED_SDL2Helpers_h_GUID_D901E282_40A2_4CB1_DA01_A88E21A45C94
#define INCLUDED_SDL2Helpers_h_GUID_D901E282_40A2_4CB1_DA01_A88E21A45C94

// Internal Includes
// - none

// Library/third-party includes
#include <SDL.h>
#include <SDL_opengl.h>

// Standard includes
#include <stdexcept>
#include <string>
#include <memory>

namespace osvr {
namespace SDL2 {
    /// @brief RAII wrapper for SDL startup/shutdown
    class Lib {
      public:
        Lib() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                throw std::runtime_error(
                    std::string("Could not initialize SDL") + SDL_GetError());
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
} // namespace SDL2
} // namespace SDL2

#endif // INCLUDED_SDL2Helpers_h_GUID_D901E282_40A2_4CB1_DA01_A88E21A45C94
