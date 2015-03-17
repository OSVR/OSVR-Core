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

#ifndef INCLUDED_InterfaceWiringFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91
#define INCLUDED_InterfaceWiringFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91

// Internal Includes
#include <osvr/Util/SharedPtr.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/DecomposeOriginalSource.h>
#include "ResolveTreeNode.h"
#include "InterfaceTree.h"

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <unordered_map>

namespace osvr {
namespace client {

    class InterfaceWiringFactory {
      public:
        typedef shared_ptr<Updatable> FactoryProduct;
        typedef std::function<FactoryProduct(common::OriginalSource const &,
                                             InterfaceTree::value_type &)>
            SpecificFactory;

        void addFactory(std::string const &name, SpecificFactory factory) {
            m_factoriesByInterface[name] = factory;
        }

        bool isKnownInterfaceType(std::string const &name) const {
            return m_factoriesByInterface.find(name) !=
                   end(m_factoriesByInterface);
        }

        FactoryProduct invokeFactory(common::OriginalSource const &source,
                                     InterfaceTree::value_type &ifaces) const {
            auto factory =
                m_factoriesByInterface.find(source.getInterfaceName());

            if (factory == end(m_factoriesByInterface)) {
                /// Unknown
                return FactoryProduct();
            }

            return (factory->second)(source, ifaces);
        }

      private:
        std::unordered_map<std::string, SpecificFactory> m_factoriesByInterface;
    };

} // namespace client
} // namespace osvr
#endif // INCLUDED_InterfaceWiringFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91
