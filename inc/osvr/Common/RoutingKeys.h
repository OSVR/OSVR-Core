/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3
#define INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace routing_keys {
        /// @brief The key in a routing directive to access the downstream
        /// destination (routing target) of the data.
        OSVR_COMMON_EXPORT const char *destination();

        /// @brief The key in a routing directive to access the upstream source
        /// of the data
        OSVR_COMMON_EXPORT const char *source();

        /// @brief the key to access a child transform
        OSVR_COMMON_EXPORT const char *child();
    } // namespace routing_keys
} // namespace common
} // namespace osvr
#endif // INCLUDED_RoutingKeys_h_GUID_2F8D4C59_8699_4BA1_86D6_BB62DC5C07F3
