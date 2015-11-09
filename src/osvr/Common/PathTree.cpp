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
#include <osvr/Common/ParseArticulation.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/JSONHelpers.h>
#include <osvr/Common/AliasProcessor.h>
#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <boost/variant/get.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    PathTree::PathTree() : m_root(PathNode::createRoot()) {}
    PathNode &PathTree::getNodeByPath(std::string const &path) {
        return pathParseAndRetrieve(*m_root, path);
    }
    PathNode &
    PathTree::getNodeByPath(std::string const &path,
                            PathElement const &finalComponentDefault) {
        auto &ret = pathParseAndRetrieve(*m_root, path);

        // Handle null elements as final component.
        elements::ifNullReplaceWith(ret.value(), finalComponentDefault);
        return ret;
    }

    PathNode const &PathTree::getNodeByPath(std::string const &path) const {
        return pathParseAndRetrieve(const_cast<PathNode const &>(*m_root),
                                    path);
    }

    void PathTree::reset() { m_root = PathNode::createRoot(); }

    /// @brief Determine if the node needs updating given that we want to add an
    /// alias there pointing to source with the given automatic status.
    static inline bool aliasNeedsUpdate(PathNode &node,
                                        std::string const &source,
                                        AliasPriority priority) {
        elements::AliasElement *elt =
            boost::get<elements::AliasElement>(&node.value());
        if (nullptr == elt) {
            /// Always replace non-aliases
            return true;
        }
        if (priority > elt->priority()) {
            /// We're a higher-priority (manual vs automatic for instance), so
            /// override
            return true;
        }
        if (priority == elt->priority() && source != elt->getSource()) {
            /// Same automatic status, different source: replace/update
            return true;
        }
        return false;
    }

    static inline bool addAliasImpl(PathNode &node, std::string const &source,
                                    AliasPriority priority) {

        if (!aliasNeedsUpdate(node, source, priority)) {
            return false;
        }
        elements::AliasElement elt;
        elt.setSource(source);
        elt.priority() = priority;
        node.value() = elt;
        return true;
    }

    static inline bool addArticulationImpl(PathNode &node,
                                           std::string const &articulationName,
                                           std::string const &boneName,
                                           std::string const &trackerPath) {

        elements::ArticulationElement elt;
        elt.setArticulationType(articulationName);
        elt.setBoneName(boneName);
        elt.setTrackerPath(trackerPath);
        node.value() = elt;
        return true;
    }

    bool addAlias(PathNode &node, std::string const &source,
                  AliasPriority priority) {
        ParsedAlias newSource(source);
        if (!newSource.isValid()) {
            /// @todo signify invalid route in some other way?
            OSVR_DEV_VERBOSE("Could not parse source: " << source);
            return false;
        }
        if (!isPathAbsolute(newSource.getLeaf())) {
            /// @todo signify not to pass relative paths here in some other way?
            OSVR_DEV_VERBOSE(
                "Source contains a relative path, not permitted: " << source);
            return false;
        }
        return addAliasImpl(node, newSource.getAlias(), priority);
    }

    bool addAliasFromRoute(PathNode &node, std::string const &route,
                           AliasPriority priority) {
        auto val = jsonParse(route);
        auto alias = applyPriorityToAlias(convertRouteToAlias(val), priority);
        return AliasProcessor().process(node, alias);
    }

    static inline std::string getAbsolutePath(PathNode &node,
                                              std::string const &path) {
        if (isPathAbsolute(path)) {
            return path;
        }
        return getFullPath(treePathRetrieve(node, path));
    }

    bool addAliasFromSourceAndRelativeDest(PathNode &node,
                                           std::string const &source,
                                           std::string const &dest,
                                           AliasPriority priority) {
        auto &aliasNode = treePathRetrieve(node, dest);
        ParsedAlias newSource(source);
        if (!newSource.isValid()) {
            /// @todo signify invalid route in some other way?
            OSVR_DEV_VERBOSE("Could not parse source: " << source);
            return false;
        }
        auto absSource = getAbsolutePath(node, newSource.getLeaf());
        newSource.setLeaf(absSource);
        return addAliasImpl(aliasNode, newSource.getAlias(), priority);
    }

    bool addArticulation(PathNode &node, std::string const &source,
                         std::string const &dest) {
        auto &articulationNode = treePathRetrieve(node, dest);
        ParsedArticulation articulationData(source);
        if (!articulationData.isValid()) {
            /// @todo signify invalid route in some other way?
            OSVR_DEV_VERBOSE(
                "Articulation spec missing tracker path data: " << source);
            return false;
        }

        auto fullTrackerPath =
            getAbsolutePath(node, articulationData.getTrackerPath());
        articulationData.setTrackerPath(fullTrackerPath);
        return addArticulationImpl(
            articulationNode, articulationData.getArticulationType(),
            articulationData.getBoneName(), articulationData.getTrackerPath());
    }

    bool isPathAbsolute(std::string const &source) {
        return !source.empty() && source.at(0) == getPathSeparatorCharacter();
    }

    void clonePathTree(PathTree const &src, PathTree &dest) {
        auto nodes = pathTreeToJson(src);
        jsonToPathTree(dest, nodes);
    }
} // namespace common
} // namespace osvr
