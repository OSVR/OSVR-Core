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
#include <osvr/Routing/PathNode.h>
#include <osvr/Routing/Constants.h>
#include <osvr/Routing/PathElementTools.h>

// Library/third-party includes
// - none

// Standard includes
#include <sstream>

namespace osvr {
namespace routing {
    const char *getTypeName(PathNode const &node) {
        return elements::getTypeName(node.value());
    }
    static inline void buildPathRecursively(PathNode const &node,
                                            std::ostream &os) {
        auto parent = node.getParent();
        if (parent) {
            buildPathRecursively(*parent, os);
        }
        if (!node.isRoot()) {
            os << getPathSeparator() << node.getName();
        }
    }
    std::string getFullPath(PathNode const &node) {
        std::ostringstream os;
        buildPathRecursively(node, os);
        return os.str();
    }
} // namespace routing
} // namespace osvr