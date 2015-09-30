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

#ifndef INCLUDED_RemoteHandlerFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91
#define INCLUDED_RemoteHandlerFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/InterfaceList.h>
#include <osvr/Common/ClientContext_fwd.h>
#include <osvr/Client/RemoteHandler.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <unordered_map>

namespace osvr {
namespace client {
    class VRPNConnectionCollection;
    class RemoteHandlerFactory {
      public:
        typedef shared_ptr<RemoteHandler> FactoryProduct;
        typedef std::function<FactoryProduct(
            common::OriginalSource const &, common::InterfaceList &,
            common::ClientContext &)> SpecificFactory;

        void addFactory(std::string const &name, SpecificFactory factory) {
            m_factoriesByInterface[name] = factory;
        }

        bool isKnownInterfaceType(std::string const &name) const {
            return m_factoriesByInterface.find(name) !=
                   end(m_factoriesByInterface);
        }

        FactoryProduct invokeFactory(common::OriginalSource const &source,
                                     common::InterfaceList &ifaces,
                                     common::ClientContext &ctx) const {
            auto factory =
                m_factoriesByInterface.find(source.getInterfaceName());

            if (factory == end(m_factoriesByInterface)) {
                /// Unknown
                return FactoryProduct();
            }

            return (factory->second)(source, ifaces, ctx);
        }

      private:
        std::unordered_map<std::string, SpecificFactory> m_factoriesByInterface;
    };

    /// @brief Populates a RemoteHandlerFactory with each of the specific
    /// factories included with OSVR.
    OSVR_CLIENT_EXPORT void
    populateRemoteHandlerFactory(RemoteHandlerFactory &factory,
                                 VRPNConnectionCollection const &conns);

} // namespace client
} // namespace osvr
#endif // INCLUDED_RemoteHandlerFactory_h_GUID_3B3394C0_DADA_4BAA_3EDD_6CDA96760D91
