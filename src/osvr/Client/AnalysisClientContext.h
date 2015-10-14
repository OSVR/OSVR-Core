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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_AnalysisClientContext_h_GUID_6B2A41C3_C718_45AD_223E_4271ABBA8427
#define INCLUDED_AnalysisClientContext_h_GUID_6B2A41C3_C718_45AD_223E_4271ABBA8427


// Internal Includes
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Util/TimeValue_fwd.h>
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Client/RemoteHandlerFactory.h>
#include <osvr/Client/ClientInterfaceObjectManager.h>
#include <osvr/Common/PathTreeOwner.h>
#include "VRPNConnectionCollection.h"

// Library/third-party includes
// - none

// Standard includes
// - none


namespace osvr {
	namespace client {

		class AnalysisClientContext : public ::OSVR_ClientContextObject {
		public:
			AnalysisClientContext(const char appId[], const char host[], vrpn_ConnectionPtr const& conn, common::ClientContextDeleter del);
			virtual ~AnalysisClientContext();

		private:
			void m_update() override;
			void m_sendRoute(std::string const &route) override;

			/// @brief Called with each new interface object before it is returned
			/// to the client.
			void
				m_handleNewInterface(common::ClientInterfacePtr const &iface) override;

			/// @brief Called with each interface object to be released/deleted
			/// after it is removed from the context's list of interfaces but before
			/// it is deleted.
			void m_handleReleasingInterface(
				common::ClientInterfacePtr const &iface) override;

			common::PathTree const &m_getPathTree() const override;

			bool m_getStatus() const override;

			/// @brief the vrpn_Connection corresponding to m_host
			vrpn_ConnectionPtr m_mainConn;

			/// @brief The "OSVR" system device for control messages
			common::BaseDevicePtr m_systemDevice;

			/// @brief The system component providing access to sending/receiving
			/// control messages.
			common::SystemComponent *m_systemComponent;

			/// @brief All open VRPN connections, keyed by host
			VRPNConnectionCollection m_vrpnConns;

			/// @brief Object owning a path tree.
			common::PathTreeOwner m_pathTreeOwner;

			/// @brief Factory for producing remote handlers.
			RemoteHandlerFactory m_factory;

			/// @brief Manager of client interface objects and their interaction
			/// with the path tree.
			ClientInterfaceObjectManager m_ifaceMgr;
		};
	} // namespace client
} // namespace osvr

#endif // INCLUDED_AnalysisClientContext_h_GUID_6B2A41C3_C718_45AD_223E_4271ABBA8427

