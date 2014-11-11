/** @file
    @brief Header

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

#ifndef INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11
#define INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11

#ifdef OGVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <signal.h>

namespace ogvr {
namespace server {
#ifndef OGVR_DOXYGEN_EXTERNAL
    namespace detail {
        template <ShutdownHandler F> void handlerWrapper(int) { F(); }
    }
#endif // #ifndef OGVR_DOXYGEN_EXTERNAL

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
} // namespace ogvr

#endif // OGVR_USE_POSIX_SIGNAL_SHUTDOWN_HANDLER

#endif // INCLUDED_RegisterShutdownHandlerPOSIXSignal_h_GUID_0B85D38D_08A2_49AA_7FA4_1BED1048FD11
