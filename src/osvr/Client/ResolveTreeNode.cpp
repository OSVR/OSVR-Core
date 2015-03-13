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
#include <osvr/Client/ResolveTreeNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/variant.hpp>
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    Updatable::~Updatable() {}
    class TraverseRouteVisitor
        : public boost::static_visitor<InterfaceWiringFactory::FactoryProduct>,
          boost::noncopyable {
      public:
        TraverseRouteVisitor(common::PathTree &tree, common::PathNode &node,
                             ClientInterfacePtr const &iface,
                             InterfaceWiringFactory const &factory)
            : boost::static_visitor<InterfaceWiringFactory::FactoryProduct>(),
              m_tree(tree), m_node(node), m_iface(iface), m_factory(factory) {
            OSVR_DEV_VERBOSE("Traversing " << common::getFullPath(node));
        }
        template <typename T>
        InterfaceWiringFactory::FactoryProduct operator()(T const &) {
            // Can't handle it.
            return InterfaceWiringFactory::FactoryProduct();
        }

        InterfaceWiringFactory::FactoryProduct
        operator()(common::elements::AliasElement const &elt) {
            // This is an alias.
            /// @todo handle transforms
            auto &source = m_tree.getNodeByPath(elt.getSource());
            return traverseRoute(m_tree, source, m_iface, m_factory);
        }

      private:
        common::PathTree &m_tree;
        common::PathNode &m_node;
        ClientInterfacePtr const &m_iface;
        InterfaceWiringFactory const &m_factory;
    };

    InterfaceWiringFactory::FactoryProduct
    traverseRoute(common::PathTree &tree, common::PathNode &node,
                  ClientInterfacePtr const &iface,
                  InterfaceWiringFactory const &factory) {
        TraverseRouteVisitor visitor(tree, node, iface, factory);

        return boost::apply_visitor(visitor, node.value());
    }
} // namespace client
} // namespace osvr