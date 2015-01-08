/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB
#define INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB

// Internal Includes
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Contains methods to retrieve the constant strings identifying
    /// message or sender types.
    namespace messagekeys {
        /// @brief The sender type for system messages.
        OSVR_UTIL_EXPORT const char *systemSender();
        /// @brief Return the string identifying full routing data messages
        OSVR_UTIL_EXPORT const char *routingData();
        /// @brief The sender type for client-to-server messages.
        OSVR_UTIL_EXPORT const char *appSender();
        /// @brief Return the string identifying individual (non-persistent)
        /// route additions/updates from client to server
        OSVR_UTIL_EXPORT const char *routeUpdate();
    } // namespace messagekeys
} // namespace util
} // namespace osvr

#endif // INCLUDED_MessageKeys_h_GUID_ADE6EC53_FA90_4393_5840_C63EFA7C57AB
