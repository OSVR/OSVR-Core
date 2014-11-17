/** @file
    @brief Header to register a handler for cross-platform shutdown/terminate
   signals.  WARNING: includes `windows.h` on Windows platforms, can't prevent
   it!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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
