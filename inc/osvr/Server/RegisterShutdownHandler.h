/** @file
    @brief Header to register a handler for cross-platform shutdown/terminate
   signals.  WARNING: includes `windows.h` on Windows platforms, can't prevent
   it!

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

#ifndef INCLUDED_RegisterShutdownHandler_h_GUID_3663ABF6_8788_4BB7_CA9F_3609E2EC7009
#define INCLUDED_RegisterShutdownHandler_h_GUID_3663ABF6_8788_4BB7_CA9F_3609E2EC7009

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace server {
    /// @brief The type of a shutdown handler callback.
    typedef void (*ShutdownHandler)();
} // namespace server
} // namespace osvr

#if !defined(OSVR_USE_WIN32_SHUTDOWN_HANDLER) &&                               \
    !defined(OSVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER)

#ifdef _WIN32
#define OSVR_USE_WIN32_SHUTDOWN_HANDLER
#else
/// @todo the world isn't just Win32 and POSIX signal()
#define OSVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER
#endif

#endif // !defined any config flag

#if defined(OSVR_USE_WIN32_SHUTDOWN_HANDLER)
#include <osvr/Server/RegisterShutdownHandlerWin32.h>

#elif defined(OSVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER)
#include <osvr/Server/RegisterShutdownHandlerPOSIXSignal.h>
#endif

#endif // INCLUDED_RegisterShutdownHandler_h_GUID_3663ABF6_8788_4BB7_CA9F_3609E2EC7009
