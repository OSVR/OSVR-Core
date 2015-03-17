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
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/DecomposeOriginalSource.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes

namespace osvr {
namespace client {
    class Updatable {
      public:
        virtual ~Updatable();
        virtual void update() = 0;
    };

#if 0
    class WiringData {
      public:
        WiringData(common::PathTree &pathTree, InterfaceTree &ifaceTree,
                   InterfaceWiringFactory const &factory);
        common::PathTree &getPathTree() { return m_pathTree; }
        InterfaceTree &getInterfaceTree() { return m_ifaceTree; }
        InterfaceWiringFactory const &getFactory() { return m_factory; }

        void conditionallySetOriginalInterfacePath(std::string const &path) {
            if (m_origPath.empty()) {
                m_origPath = path;
            }
        }
        std::string const &getOriginalInterfacePath() const {
            return m_origPath;
        }

      private:
        common::PathTree &m_pathTree;
        InterfaceTree &m_ifaceTree;
        InterfaceWiringFactory const &m_factory;
        std::string m_origPath;
    };


    InterfaceWiringFactory::FactoryProduct
    traverseRoute(WiringData &data, common::PathNode &node);
#endif
    boost::optional<common::OriginalSource>
    resolveTreeNode(common::PathTree &pathTree, std::string const &path);

} // namespace client
} // namespace osvr

#endif // INCLUDED_ResolveTreeNode_h_GUID_1EAB4565_C824_4B88_113E_7E898D998E9E
