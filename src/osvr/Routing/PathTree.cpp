/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Routing/PathTree.h>
#include <osvr/Routing/PathNode.h>
#include "PathParseAndRetrieve.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace routing {
    PathTree::PathTree() : m_root(PathNode::createRoot()) {}
    PathNode &PathTree::getNodeByPath(std::string const &path) {
        return pathParseAndRetrieve(path, *m_root);
    }
} // namespace routing
} // namespace osvr