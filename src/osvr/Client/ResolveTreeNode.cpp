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
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/DecomposeOriginalSource.h>
#include <osvr/Util/Verbosity.h>

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

        OSVR_DEV_VERBOSE(
            "In ifNullTryInferFromParent for: " << common::getFullPath(node));
        if (nullptr ==
            boost::get<common::elements::NullElement>(&node.value())) {
            OSVR_DEV_VERBOSE("skipping, not null");
            /// Not null.
            return;
        }

        if (nullptr == node.getParent()) {
            OSVR_DEV_VERBOSE("skipping, no parent");
            // couldn't help, no parent.
            return;
        }
        auto const &parent = *node.getParent();

        if (nullptr ==
            boost::get<common::elements::InterfaceElement>(&(parent.value()))) {
            OSVR_DEV_VERBOSE("Skipping, parent node isn't an interface");
            return; // parent isn't an interface.
        }

        OSVR_DEV_VERBOSE("OK, replacing with sensor element");
        // So if we get here, parent is present and an interface, which means
        // that we're a sensor.
        node.value() = common::elements::SensorElement();
    }

    /// @brief Struct to bundle data used between multiple instances of a static
    /// visitor.
    struct TraversalBaggage : boost::noncopyable {
        TraversalBaggage(common::PathTree &tree_,
                         ClientInterfacePtr const &iface_,
                         InterfaceWiringFactory const &factory_)
            : tree(tree_), iface(iface_), factory(factory_) {}

        common::PathTree &tree;
        ClientInterfacePtr const &iface;
        InterfaceWiringFactory const &factory;
    };

    InterfaceWiringFactory::FactoryProduct
    traverseRoute(TraversalBaggage &baggage, common::PathNode &node);

    class TraverseRouteVisitor
        : public boost::static_visitor<InterfaceWiringFactory::FactoryProduct>,
          boost::noncopyable {
      public:
        TraverseRouteVisitor(TraversalBaggage &baggage, common::PathNode &node)
            : boost::static_visitor<InterfaceWiringFactory::FactoryProduct>(),
              m_baggage(baggage), m_node(node) {
            OSVR_DEV_VERBOSE("Traversing " << common::getFullPath(node));
        }

        /// @brief Fallback case
        template <typename T>
        InterfaceWiringFactory::FactoryProduct operator()(T const &) {
            // Can't handle it.
            OSVR_DEV_VERBOSE("Couldn't handle: node value type of "
                             << common::getTypeName(m_node));
            return InterfaceWiringFactory::FactoryProduct();
        }
        /// @todo handle other types here

        /// @brief Handle an alias element
        InterfaceWiringFactory::FactoryProduct
        operator()(common::elements::AliasElement const &elt) {
            // This is an alias.
            /// @todo handle transforms
            auto &source = m_baggage.tree.getNodeByPath(elt.getSource());
            return traverseRoute(m_baggage, source);
        }

        /// @brief Handle a sensor element
        InterfaceWiringFactory::FactoryProduct
        operator()(common::elements::SensorElement const &) {
            /// This is the end of the traversal: landed on a sensor.
            common::DecomposeOriginalSource decomp{m_node};
            BOOST_ASSERT_MSG(decomp.gotDeviceAndInterface(),
                             "Landing on a sensor means we should have an "
                             "interface and device, exceptions would be thrown "
                             "in Decompose otherwise.");
            return m_baggage.factory.invokeFactory(decomp.getInterfaceName(),
                                                   m_node, m_baggage.iface);
        }

        /// @brief Handle an interface element
        InterfaceWiringFactory::FactoryProduct
        operator()(common::elements::InterfaceElement const &) {
            /// This is the end of the traversal: landed on a interface.
            common::DecomposeOriginalSource decomp{m_node};
            BOOST_ASSERT_MSG(decomp.gotDeviceAndInterface(),
                             "Landing on an interface means we should have an "
                             "interface and device, exceptions would be thrown "
                             "in Decompose otherwise.");
            return m_baggage.factory.invokeFactory(decomp.getInterfaceName(),
                                                   m_node, m_baggage.iface);
        }

      private:
        TraversalBaggage &m_baggage;
        common::PathNode &m_node;
    };

    inline InterfaceWiringFactory::FactoryProduct
    traverseRoute(TraversalBaggage &baggage, common::PathNode &node) {

        OSVR_DEV_VERBOSE("In traverseRoute(TraversalBaggage &baggage, "
                         "common::PathNode &node)");
        // First do any inference possible here.
        ifNullTryInferFromParent(node);
        // Now visit.
        TraverseRouteVisitor visitor(baggage, node);
        return boost::apply_visitor(visitor, node.value());
    }

    InterfaceWiringFactory::FactoryProduct
    traverseRoute(common::PathTree &tree, common::PathNode &node,
                  ClientInterfacePtr const &iface,
                  InterfaceWiringFactory const &factory) {

        OSVR_DEV_VERBOSE("In traverseRoute(common::PathTree &tree, "
                         "common::PathNode &node, ClientInterfacePtr const "
                         "&iface, InterfaceWiringFactory const &factory)");
        auto baggage = TraversalBaggage{tree, iface, factory};
        return traverseRoute(baggage, node);
    }
} // namespace client
} // namespace osvr