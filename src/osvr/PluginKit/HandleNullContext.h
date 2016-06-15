/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_HandleNullContext_h_GUID_3951FC65_BC2E_49BD_CACD_978BD4D8F337
#define INCLUDED_HandleNullContext_h_GUID_3951FC65_BC2E_49BD_CACD_978BD4D8F337

// Internal Includes
#include <osvr/PluginKit/CommonC.h>
#include <osvr/Util/MacroToolsC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

/// @brief Internal macro for use in C API function implementations to check the
/// validity of a context parameter for return-code functions.
#define OSVR_PLUGIN_HANDLE_NULL_CONTEXT(FUNC, CONTEXT_NAME)                    \
    OSVR_UTIL_MULTILINE_BEGIN if (!CONTEXT_NAME) {                             \
        std::cerr << "ERROR (" << FUNC                                         \
                  << "): Null context " #CONTEXT_NAME " passed!" << std::endl; \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

/// @brief Internal macro for use in C API function implementations to check the
/// validity of a context parameter for constructor-like functions.
#define OSVR_PLUGIN_HANDLE_NULL_CONTEXT_CONSTRUCTOR(FUNC, CONTEXT_NAME)        \
    OSVR_UTIL_MULTILINE_BEGIN if (!CONTEXT_NAME) {                             \
        std::cerr << "ERROR (" << FUNC                                         \
                  << "): Null context " #CONTEXT_NAME " passed!" << std::endl; \
        return nullptr;                                                        \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

#endif // INCLUDED_HandleNullContext_h_GUID_3951FC65_BC2E_49BD_CACD_978BD4D8F337
