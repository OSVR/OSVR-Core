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

#ifndef INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
#define INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2

// Internal Includes
#include <osvr/Routing/PathElementTypes_fwd.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace routing {
    namespace elements {
        /// @brief Gets a string that indicates the type of path element. Do not
        /// use this for conditionals/comparisons unless there's really no
        /// better way! (There probably is a better way with a variant
        /// static_visitor)
        /// @param elt The element to investigate.
        const char *getTypeName(PathElement const &elt);
    } // namespace elements
} // namespace routing
} // namespace osvr
#endif // INCLUDED_PathElementTools_h_GUID_9246E2D5_1598_409F_BD30_1817FA2C1FB2
