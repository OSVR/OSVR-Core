/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/AliasProcessor.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Util/Flag.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/ParseAlias.h>
#include <osvr/Common/RoutingKeys.h>

#include "PathParseAndRetrieve.h"

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {
        static const char PRIORITY_KEY[] = "$priority";
        static const char WILDCARD_SUFFIX[] = "/*";
        static const size_t WILDCARD_SUFFIX_LEN = sizeof(WILDCARD_SUFFIX) - 1;

        /// @brief Handles wildcards with a functor of your choice: it should
        /// take in a PathNode& node, std::string const& relPath
        /// where relPath is relative to the parent of the wildcard.
        template <typename T>
        inline void applyWildcard(PathNode &node,
                                  std::string const &pathWithWildcard,
                                  T functor) {
            auto startingPath = pathWithWildcard;
            boost::algorithm::erase_tail(startingPath, WILDCARD_SUFFIX_LEN);
            auto &startingNode = treePathRetrieve(node, startingPath);
            auto absoluteStartingPath = getFullPath(startingNode);
            auto absoluteStartingPathLen = absoluteStartingPath.length();
            util::traverseWith(startingNode, [&](PathNode &node) {
                // Don't visit null nodes
                if (elements::isNull(node.value())) {
                    return;
                }
                auto visitPath = getFullPath(node);
                /// This is relative to the initial starting path stem: where we
                /// started the traversal.
                auto relPath = visitPath;
                boost::algorithm::erase_head(relPath,
                                             absoluteStartingPathLen + 1);
                functor(node, relPath);
            });
        }

        /// @brief Predicate that checks if this path contains a wildcard.
        inline bool doesPathContainWildcard(std::string const &path) {
            return boost::algorithm::ends_with(path, WILDCARD_SUFFIX);
        }

        /// @brief Implementation functor for AliasProcessor.
        class AutomaticAliases : boost::noncopyable {
          public:
            AutomaticAliases(PathNode &devNode,
                             detail::AliasProcessorOptions opts)
                : m_devNode(devNode), m_opts(opts) {}

            /// @brief Function call operator: pass any Json::Value. If it is an
            /// object with aliases, or an array of objects with aliases, it
            /// will handle them.
            /// @return whether any changes were made to the tree
            util::Flag operator()(Json::Value const &val) {
                if (val.isArray()) {
                    m_processArray(val);
                } else if (val.isObject()) {
                    m_processObject(val);
                }
                return m_flag;
            }

          private:
            /// @brief Takes in an array of objects, and passes them each, in
            /// order, to m_processObject()
            void m_processArray(Json::Value const &arr) {
                for (auto const &elt : arr) {
                    if (elt.isObject()) {
                        m_processObject(elt);
                    }
                }
            }

            /// @brief Takes in an object, retrieves the priority specified by
            /// the PRIORITY_KEY, if any, then passes all other entries in
            /// the object to m_processEntry() along with the default or
            /// retrieved priority.
            void m_processObject(Json::Value const &obj) {
                AliasPriority priority{m_opts.defaultPriority};
                if (obj.isMember(PRIORITY_KEY)) {
                    priority =
                        static_cast<AliasPriority>(obj[PRIORITY_KEY].asInt());
                }
                for (auto const &key : obj.getMemberNames()) {
                    if (PRIORITY_KEY == key) {
                        continue;
                    }
                    m_processEntry(key, obj[key], priority);
                }
            }

            /// @brief Process a name:value entry in an alias description
            /// object, with the given priority. Calls m_processSingleEntry() to
            /// actually complete its work.
            void m_processEntry(std::string const &path,
                                Json::Value const &source,
                                AliasPriority priority) {
                if (!m_opts.permitRelativePath && !isPathAbsolute(path)) {
                    OSVR_DEV_VERBOSE(
                        "Got a non-permitted relative path: " << path);
                    return;
                }
                ParsedAlias parsedSource(source);
                auto leaf = parsedSource.getLeaf();
                if (!m_opts.permitRelativeSource && !isPathAbsolute(leaf)) {
                    OSVR_DEV_VERBOSE(
                        "Got a non-permitted relative source leaf: " << leaf);
                    return;
                }
                if (!doesPathContainWildcard(leaf)) {
                    /// Handle the simple ones first.
                    m_processSingleEntry(path, parsedSource.getAlias(),
                                         priority);
                    return;
                }

                /// OK, handle wildcard here
                if (!m_opts.permitWildcard) {
                    OSVR_DEV_VERBOSE(
                        "Got a non-permitted wildcard in the source leaf of: "
                        << parsedSource.getAlias());
                }

                if (parsedSource.isSimple()) {
                    applyWildcard(
                        m_devNode, leaf,
                        [&](PathNode &node, std::string const &relPath) {
                            m_processSingleEntry(path + getPathSeparator() +
                                                     relPath,
                                                 getFullPath(node), priority);
                        });
                    return;
                }

                applyWildcard(m_devNode, leaf, [&](PathNode &node,
                                                   std::string const &relPath) {
                    parsedSource.setLeaf(getFullPath(node));
                    m_processSingleEntry(path + getPathSeparator() + relPath,
                                         parsedSource.getAlias(), priority);
                });
            }

            /// @brief Called for each individual alias path to be processed for
            /// (and potentially added to/updated in) the path tree.
            void m_processSingleEntry(std::string const &path,
                                      std::string const &source,
                                      AliasPriority priority) {
                m_flag += addAliasFromSourceAndRelativeDest(m_devNode, source,
                                                            path, priority);
            }

            PathNode &m_devNode;
            detail::AliasProcessorOptions m_opts;
            util::Flag m_flag;
        };

    } // namespace

    bool AliasProcessor::process(PathNode &node, Json::Value const &val) {
        AutomaticAliases processor{node, m_opts};
        return processor(val).get();
    }

    Json::Value createJSONAlias(std::string const &path,
                                Json::Value const &destination) {
        Json::Value ret{Json::nullValue};
        if (path.empty()) {
            return ret;
        }
        if (destination.isNull()) {
            return ret;
        }
        ret = Json::objectValue;
        ret[path] = destination;
        return ret;
    }

    Json::Value convertRouteToAlias(Json::Value const &val) {
        Json::Value ret{val};
        if (!val.isObject()) {
            // Can't be a route if it's not an object.
            return ret;
        }
        if (val.isMember(routing_keys::destination()) &&
            val.isMember(routing_keys::source())) {
            // By golly this is an old-fashioned route.
            ret = createJSONAlias(val[routing_keys::destination()].asString(),
                                  val[routing_keys::source()]);
        }
        return ret;
    }

    Json::Value applyPriorityToAlias(Json::Value const &alias,
                                     AliasPriority priority) {
        Json::Value ret{alias};
        if (ret.isObject()) {
            ret[PRIORITY_KEY] = priority;
        }
        return ret;
    }
} // namespace common
} // namespace osvr