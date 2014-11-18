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

#ifndef INCLUDED_PathNodePtr_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
#define INCLUDED_PathNodePtr_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    class PathNode;
    typedef shared_ptr<PathNode> PathNodePtr;
    typedef weak_ptr<PathNode> PathNodeWeakPtr;
} // namespace client
} // namespace osvr

#endif // INCLUDED_PathNodePtr_h_GUID_7AEB7FE7_1E26_478B_61DD_D39292047B03
