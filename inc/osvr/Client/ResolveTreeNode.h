/** @file
    @brief Header

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

#ifndef INCLUDED_ResolveTreeNode_h_GUID_1EAB4565_C824_4B88_113E_7E898D998E9E
#define INCLUDED_ResolveTreeNode_h_GUID_1EAB4565_C824_4B88_113E_7E898D998E9E

// Internal Includes
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Client/ClientInterfacePtr.h>

// Library/third-party includes
#include <boost/any.hpp>

// Standard includes
#include <functional>
#include <unordered_map>

namespace osvr {
namespace client {
    class Updatable {
      public:
        virtual ~Updatable();
        virtual void update() = 0;
    };
    class InterfaceWiringFactory {
      public:
        typedef shared_ptr<Updatable> FactoryProduct;
        typedef std::function<FactoryProduct(
            common::PathNode &, ClientInterfacePtr const &)> SpecificFactory;

        void addFactory(std::string const &name, SpecificFactory factory) {
            m_factoriesByInterface[name] = factory;
        }

        bool isKnownInterfaceType(std::string const &name) const {
            return m_factoriesByInterface.find(name) !=
                   end(m_factoriesByInterface);
        }

        FactoryProduct invokeFactory(std::string const &name,
                                     common::PathNode &node,
                                     ClientInterfacePtr const &iface) const {
            auto factory = m_factoriesByInterface.find(name);

            if (factory == end(m_factoriesByInterface)) {
                /// Unknown
                return FactoryProduct();
            }

            return (factory->second)(node, iface);
        }

      private:
        std::unordered_map<std::string, SpecificFactory> m_factoriesByInterface;
    };

    InterfaceWiringFactory::FactoryProduct
    traverseRoute(common::PathTree &tree, common::PathNode &node,
                  ClientInterfacePtr const &iface,
                  InterfaceWiringFactory const &factory);

} // namespace client
} // namespace osvr

#endif // INCLUDED_ResolveTreeNode_h_GUID_1EAB4565_C824_4B88_113E_7E898D998E9E
