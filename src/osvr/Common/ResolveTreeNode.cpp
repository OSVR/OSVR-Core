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
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/ParseAlias.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/ResolveTreeNode.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <json/reader.h>
#include <json/value.h>

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
        }

        /// @brief Handle an alias element
        void operator()(elements::AliasElement const &elt) {
            // This is an alias.
            ParsedAlias parsed(elt.getSource());
            if (!parsed.isValid()) {
                OSVR_DEV_VERBOSE("Couldn't parse alias: " << elt.getSource());
                return;
            }
            /// @todo update the element with the normalized source?
            if (!parsed.isSimple()) {
                // Not simple: store the full string as a transform.
                m_source.nestTransform(parsed.getAliasValue());
            }
            m_recurse(parsed.getLeaf());
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