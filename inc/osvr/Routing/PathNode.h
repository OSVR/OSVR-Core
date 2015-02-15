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

#ifndef INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0
#define INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0

// Internal Includes
#include <osvr/Routing/PathNode_fwd.h>
#include <osvr/Routing/PathElementTypes.h> ///< @todo can we split out this include? I don't think all consumers of this header need it.
#include <osvr/Util/TreeNode.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace routing {
    /// @brief Gets the absolute path for the given node.
    ///
    /// @ingroup Routing
    OSVR_ROUTING_EXPORT std::string getFullPath(PathNode const &node);
} // namespace routing
} // namespace osvr

#endif // INCLUDED_PathNode_h_GUID_0A160656_BCAF_4C96_EF3F_5209470644B0
