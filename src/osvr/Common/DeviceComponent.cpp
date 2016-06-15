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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/DeviceComponent.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    DeviceComponent::DeviceComponent() : m_parent(nullptr) {}

    void DeviceComponent::recordParent(Parent &dev) {
        BOOST_ASSERT_MSG(nullptr == m_parent,
                         "recordParent should only be called once!");
        m_parent = &dev;
        m_parentSet();
    }

    void DeviceComponent::update() { m_update(); }

    bool DeviceComponent::m_hasParent() const { return nullptr != m_parent; }

    DeviceComponent::Parent &DeviceComponent::m_getParent() {
        return *m_parent;
    }

    void DeviceComponent::m_registerHandler(vrpn_MESSAGEHANDLER handler,
                                            void *userdata,
                                            RawMessageType const &msgType) {
        auto h = make_shared<MessageHandler<BaseDeviceMessageHandleTraits> >(
            handler, userdata, msgType);
        h->registerHandler(&m_getParent());
        m_messageHandlers.push_back(h);
    }
    void DeviceComponent::m_update() {}
} // namespace common
} // namespace osvr
