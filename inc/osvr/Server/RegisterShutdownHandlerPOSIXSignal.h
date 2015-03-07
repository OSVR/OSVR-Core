/** @file
    @brief Implementation header included by RegisterShutdownHandler.h on
   platforms providing `signal()`.

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

#ifndef INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11
#define INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11

#ifdef OSVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <signal.h>

namespace osvr {
namespace server {
#ifndef OSVR_DOXYGEN_EXTERNAL
    namespace detail {
        template <ShutdownHandler F> void handlerWrapper(int) { F(); }
    }  // namespace detail
#endif // #ifndef OSVR_DOXYGEN_EXTERNAL

    /// @brief Register a function to be called when some attempt to close the
    /// console app occurs.
    ///
    /// @tparam F The address of a shutdown handler function: no params, no
    /// return value.
    template <ShutdownHandler F> inline void registerShutdownHandler() {
        signal(SIGINT, &detail::handlerWrapper<F>);
        signal(SIGKILL, &detail::handlerWrapper<F>);
        signal(SIGTERM, &detail::handlerWrapper<F>);
        signal(SIGPIPE, &detail::handlerWrapper<F>);
    }
} // namespace server
} // namespace osvr

#endif // OSVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER

#endif // INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11
