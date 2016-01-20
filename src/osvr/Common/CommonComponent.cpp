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
#include <osvr/Common/CommonComponent.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {

        // These messages all must match the ones in VRPN exactly.
        /// @todo add tests to verify that these message identifiers match VRPN
        /// string constants.

        const char *VRPNPing::identifier() { return "vrpn_Base ping_message"; }
        const char *VRPNPong::identifier() { return "vrpn_Base pong_message"; }
        const char *VRPNGotFirstConnection::identifier() {
            return "VRPN_Connection_Got_First_Connection";
        }
        const char *VRPNGotConnection::identifier() {
            return "VRPN_Connection_Got_Connection";
        }
        const char *VRPNDroppedConnection::identifier() {
            return "VRPN_Connection_Dropped_Connection";
        }
        const char *VRPNDroppedLastConnection::identifier() {
            return "VRPN_Connection_Dropped_Last_Connection";
        }

    } // namespace messages
    shared_ptr<CommonComponent> CommonComponent::create() {
        shared_ptr<CommonComponent> ret(new CommonComponent);
        return ret;
    }

    void CommonComponent::registerPingHandler(Handler const &handler) {
        if (m_pingHandlers.empty()) {
            m_registerHandler(&CommonComponent::m_handlePing, this,
                              ping.getMessageType());
        }
        m_pingHandlers.push_back(handler);
    }

    void CommonComponent::registerPongHandler(Handler const &handler) {
        if (m_pongHandlers.empty()) {
            m_registerHandler(&CommonComponent::m_handlePing, this,
                              pong.getMessageType());
        }
        m_pongHandlers.push_back(handler);
    }

    CommonComponent::CommonComponent() {}
    void CommonComponent::m_parentSet() {
        m_getParent().registerMessageType(ping);
        m_getParent().registerMessageType(pong);
    }
    int CommonComponent::m_handlePing(void *userdata, vrpn_HANDLERPARAM) {
        auto self = static_cast<CommonComponent *>(userdata);
        for (auto const &cb : self->m_pingHandlers) {
            cb();
        }
        return 0;
    }
    int CommonComponent::m_handlePong(void *userdata, vrpn_HANDLERPARAM) {
        auto self = static_cast<CommonComponent *>(userdata);
        for (auto const &cb : self->m_pongHandlers) {
            cb();
        }
        return 0;
    }

} // namespace common
} // namespace osvr