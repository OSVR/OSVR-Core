/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Routing/PathTree.h>
#include <osvr/Routing/PathElementTypes.h>
#include <osvr/Util/TreeNode.h>
#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <boost/variant/variant.hpp>

// Standard includes
// - none

namespace osvr {
namespace routing {
    PathTree::PathTree() : m_root(Node::createRoot()) {}
    PathTree::Node &PathTree::getNodeByPath(std::string const &path) {
        return pathParseAndRetrieve(path, *m_root);
    }

} // namespace routing
} // namespace osvr