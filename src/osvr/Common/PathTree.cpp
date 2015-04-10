/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/RouteContainer.h>
#include <osvr/Common/ParseAlias.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Util/Verbosity.h>
#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <boost/variant/get.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    PathTree::PathTree() : m_root(PathNode::createRoot()) {}
    PathNode &PathTree::getNodeByPath(std::string const &path) {
        return detail::pathParseAndRetrieve(*m_root, path);
    }
    PathNode &
    PathTree::getNodeByPath(std::string const &path,
                            PathElement const &finalComponentDefault) {
        auto &ret = detail::pathParseAndRetrieve(*m_root, path);

        // Handle null elements as final component.
        elements::ifNullReplaceWith(ret.value(), finalComponentDefault);
        return ret;
    }

    void PathTree::reset() { m_root = PathNode::createRoot(); }

    /// @brief Determine if the node needs updating given that we want to add an
    /// alias there pointing to source with the given automatic status.
    static inline bool aliasNeedsUpdate(PathNode &node,
                                        std::string const &source,
                                        bool automatic) {
        elements::AliasElement *elt =
            boost::get<elements::AliasElement>(&node.value());
        if (nullptr == elt) {
            /// Always replace non-aliases
            return true;
        }
        if (!automatic && elt->getAutomatic()) {
            /// We're a manual alias, and there is only an automatic, so we
            /// override
            return true;
        }
        if (automatic == elt->getAutomatic() && source != elt->getSource()) {
            /// Same automatic status, different source: replace/update
            return true;
        }
        return false;
    }

    /// @brief Make node an alias pointing to source, with the given automatic
    /// status, if it needs updating.
    ///
    /// @return true if the node was changed
    static inline bool addAlias(PathNode &node, std::string const &source,
                                bool automatic) {

        if (!aliasNeedsUpdate(node, source, automatic)) {
            return false;
        }
        elements::AliasElement elt;
        elt.setSource(source);
        elt.getAutomatic() = automatic;
        node.value() = elt;
        return true;
    }

    bool addAliasFromRoute(PathNode &node, std::string const &route,
                           bool automatic) {
        auto path = common::RouteContainer::getDestinationFromString(route);
        auto &aliasNode = detail::treePathRetrieve(node, path);
        ParsedAlias newSource(route);
        if (!newSource.isValid()) {
            /// @todo signify invalid route in some other way?
            OSVR_DEV_VERBOSE("Could not parse route as a source: " << route);
            return false;
        }
        if (!isPathAbsolute(newSource.getLeaf())) {
            /// @todo signify not to pass relative paths here in some other way?
            OSVR_DEV_VERBOSE(
                "Route contains a relative path, not permitted: " << route);
            return false;
        }
        OSVR_DEV_VERBOSE("addAliasFromRoute:\n\tRoute: "
                         << route
                         << "\n\tNew source: " << newSource.getAlias());
        OSVR_DEV_VERBOSE("addAliasFromRoute: " << getFullPath(aliasNode)
                                               << " -> "
                                               << newSource.getLeaf());
        return addAlias(aliasNode, newSource.getAlias(), automatic);
    }

    static inline std::string getAbsolutePath(PathNode &node,
                                              std::string const &path) {
        if (isPathAbsolute(path)) {
            return path;
        }
        return getFullPath(detail::treePathRetrieve(node, path));
    }

    bool addAliasFromSourceAndRelativeDest(PathNode &node,
                                           std::string const &source,
                                           std::string const &dest,
                                           bool automatic) {
        auto &aliasNode = detail::treePathRetrieve(node, dest);
        ParsedAlias newSource(source);
        if (!newSource.isValid()) {
            /// @todo signify invalid route in some other way?
            OSVR_DEV_VERBOSE("Could not parse source: " << source);
            return false;
        }
        auto absSource = getAbsolutePath(node, newSource.getLeaf());
        newSource.setLeaf(absSource);
        OSVR_DEV_VERBOSE("addAliasFromSourceAndRelativeDest:\n\tOld source: "
                         << source
                         << "\n\tNew source: " << newSource.getAlias());
        OSVR_DEV_VERBOSE("addAliasFromSourceAndRelativeDest: "
                         << getFullPath(aliasNode) << " -> " << absSource);
        return addAlias(aliasNode, newSource.getAlias(), automatic);
    }

    bool isPathAbsolute(std::string const &source) {
        return !source.empty() && source.at(0) == getPathSeparatorCharacter();
    }
} // namespace common
} // namespace osvr
