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

#ifndef INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C
#define INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C

// Internal Includes
#include <osvr/Routing/PathNode_fwd.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace routing {
    PathNode &pathParseAndRetrieve(std::string const &path, PathNode &root);
} // namespace routing
} // namespace osvr

#endif // INCLUDED_PathParseAndRetrieve_h_GUID_C451663C_0711_4B85_2011_61D26E5C237C
