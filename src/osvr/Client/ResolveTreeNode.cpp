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
#include "ResolveTreeNode.h"
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/DecomposeOriginalSource.h>
#include <osvr/Util/Verbosity.h>
#include "InterfaceTree.h"

// Library/third-party includes
#include <boost/variant.hpp>
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    Updatable::~Updatable() {}

    /// @brief Given a node, if it's null, try to infer from the parent what it
    /// should be.
    ///
    /// Right now can only infer that the children of an interface are sensors.
    inline void ifNullTryInferFromParent(common::PathNode &node) {
        if (nullptr ==
            boost::get<common::elements::NullElement>(&node.value())) {
            /// Not null.
            return;
        }

        if (nullptr == node.getParent()) {
            // couldn't help, no parent.
            return;
        }
        auto const &parent = *node.getParent();

        if (nullptr ==
            boost::get<common::elements::InterfaceElement>(&(parent.value()))) {
            return; // parent isn't an interface.
        }
        // So if we get here, parent is present and an interface, which means
        // that we're a sensor.
        node.value() = common::elements::SensorElement();
    }
    // Forward declaration
    void resolveTreeNodeImpl(common::PathTree &pathTree,
                             std::string const &path,
                             common::OriginalSource &source);

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
        /// @todo handle other types here

        /// @brief Handle an alias element
        void operator()(common::elements::AliasElement const &elt) {
            // This is an alias.
            /// @todo handle transforms
            auto &source = m_getPathTree().getNodeByPath(elt.getSource());
            m_recurse(elt.getSource());
        }

        /// @brief Handle a sensor element
        void operator()(common::elements::SensorElement const &) {
            /// This is the end of the traversal: landed on a sensor.
            m_decompose();
            BOOST_ASSERT_MSG(m_source.isResolved(),
                             "Landing on a sensor means we should have an "
                             "interface and device, exceptions would be thrown "
                             "in Decompose otherwise.");
        }

        /// @brief Handle an interface element
        void operator()(common::elements::InterfaceElement const &) {
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
        common::PathTree &m_getPathTree() { return m_tree; }

        common::PathTree &m_tree;
        common::PathNode &m_node;
        common::OriginalSource &m_source;
    };

    inline static void resolveTreeNodeImpl(common::PathTree &pathTree,
                                           std::string const &path,
                                           common::OriginalSource &source) {
        OSVR_DEV_VERBOSE("Traversing " << path);
        auto &node = pathTree.getNodeByPath(path);

        // First do any inference possible here.
        ifNullTryInferFromParent(node);

        // Now visit.
        TreeResolutionVisitor visitor(pathTree, node, source);
        boost::apply_visitor(visitor, node.value());
    }

    boost::optional<common::OriginalSource>
    resolveTreeNode(common::PathTree &pathTree, std::string const &path) {
        common::OriginalSource source;
        resolveTreeNodeImpl(pathTree, path, source);
        if (source.isResolved()) {
            return source;
        }
        return boost::optional<common::OriginalSource>();
    }
} // namespace client
} // namespace osvr