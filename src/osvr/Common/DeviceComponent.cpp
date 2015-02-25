/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/BaseDevice.h>

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
        auto h = make_shared<MessageHandler<BaseDeviceMessage> >(
            handler, userdata, msgType);
        h->registerHandler(&m_getParent());
        m_messageHandlers.push_back(h);
    }
    void DeviceComponent::m_update() {}
} // namespace common
} // namespace osvr
