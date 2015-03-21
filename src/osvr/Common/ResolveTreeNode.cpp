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
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/JSONTransformVisitor.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <sstream>

namespace osvr {
namespace common {

    /// @brief Given a node, if it's null, try to infer from the parent what it
    /// should be.
    ///
    /// Right now can only infer that the children of an interface are sensors.
    inline void ifNullTryInferFromParent(common::PathNode &node) {
        if (nullptr == boost::get<elements::NullElement>(&node.value())) {
            /// Not null.
            return;
        }

        if (nullptr == node.getParent()) {
            // couldn't help, no parent.
            return;
        }
        auto const &parent = *node.getParent();

        if (nullptr ==
            boost::get<elements::InterfaceElement>(&(parent.value()))) {
            return; // parent isn't an interface.
        }
        // So if we get here, parent is present and an interface, which means
        // that we're a sensor.
        node.value() = elements::SensorElement();
    }

    inline std::string getPathFromOldRouteSource(Json::Value obj) {
        std::ostringstream ret;
        if (obj.isObject() && obj.isMember("tracker")) {
            auto tracker = obj["tracker"].asString();
            if (tracker.front() != '/') {
                ret << "/";
            }
            ret << tracker;
            ret << "/tracker";
            if (obj.isMember("sensor")) {
                ret << "/";
                ret << obj["sensor"].asInt();
            }
        }
        return ret.str();
    }

    // Forward declaration
    void resolveTreeNodeImpl(PathTree &pathTree, std::string const &path,
                             OriginalSource &source);

    class TreeResolutionVisitor : public boost::static_visitor<>,
                                  boost::noncopyable {
      public:
        TreeResolutionVisitor(common::PathTree &tree, common::PathNode &node,
                              common::OriginalSource &source)
            : boost::static_visitor<>(), m_tree(tree), m_node(node),
              m_source(source) {}

        /// @brief Fallback case
        template <typename T> void operator()(T const &) {
            // Can't handle it.
            OSVR_DEV_VERBOSE("Couldn't handle: node value type of "
                             << common::getTypeName(m_node));
        }

        /// @brief Handle an alias element
        void operator()(elements::AliasElement const &elt) {
            // This is an alias.
            /// @todo handle transforms
            auto src = elt.getSource();
            Json::Value val;
            Json::Reader reader;
            if (reader.parse(src, val)) {
                if (val.isString()) {
                    // Assume a string is just a string.
                    m_recurse(val.asString());
                    return;
                }
                if (val.isObject()) {
                    // Assume an object means a transform.
                    m_source.setTransform(src);

                    JSONTransformVisitor xformParse(val);
                    auto leaf = xformParse.getLeaf();

                    if (leaf.isString()) {
                        m_recurse(leaf.asString());
                        return;
                    }

                    auto trackerEquiv = getPathFromOldRouteSource(leaf);
                    if (!trackerEquiv.empty()) {
                        m_recurse(trackerEquiv);
                        return;
                    }

                    OSVR_DEV_VERBOSE("Couldn't handle transform leaf: "
                                     << leaf.toStyledString());
                    /// @todo finish
                    return;
                }
            }
            // If we couldn't parse, just recurse directly on the string.
            m_recurse(src);
        }

        /// @brief Handle a sensor element
        void operator()(elements::SensorElement const &) {
            /// This is the end of the traversal: landed on a sensor.
            m_decompose();
            BOOST_ASSERT_MSG(m_source.isResolved(),
                             "Landing on a sensor means we should have an "
                             "interface and device, exceptions would be thrown "
                             "in Decompose otherwise.");
        }

        /// @brief Handle an interface element
        void operator()(elements::InterfaceElement const &) {
            /// This is the end of the traversal: landed on a interface.
            m_decompose();
            BOOST_ASSERT_MSG(m_source.isResolved(),
                             "Landing on an interface means we should have an "
                             "interface and device, exceptions would be thrown "
                             "in Decompose otherwise.");
        }

      private:
        void m_decompose() { m_source.decompose(m_node); }
        void m_recurse(std::string const &path) {
            resolveTreeNodeImpl(m_tree, path, m_source);
        }
        PathTree &m_getPathTree() { return m_tree; }

        PathTree &m_tree;
        PathNode &m_node;
        OriginalSource &m_source;
    };

    inline void resolveTreeNodeImpl(PathTree &pathTree, std::string const &path,
                                    OriginalSource &source) {
        OSVR_DEV_VERBOSE("Traversing " << path);
        auto &node = pathTree.getNodeByPath(path);

        // First do any inference possible here.
        ifNullTryInferFromParent(node);

        // Now visit.
        TreeResolutionVisitor visitor(pathTree, node, source);
        boost::apply_visitor(visitor, node.value());
    }

    boost::optional<OriginalSource> resolveTreeNode(PathTree &pathTree,
                                                    std::string const &path) {
        OriginalSource source;
        resolveTreeNodeImpl(pathTree, path, source);
        if (source.isResolved()) {
            return source;
        }
        return boost::optional<OriginalSource>();
    }
} // namespace common
} // namespace osvr