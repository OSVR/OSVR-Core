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
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/DeviceComponent.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace common {

    BaseDevice::BaseDevice() {}
    BaseDevice::~BaseDevice() {}

    void BaseDevice::addComponent(DeviceComponentPtr component) {
        if (!component) {
            throw std::logic_error(
                "Tried to add a null component pointer to a base device!");
        }
        m_components.push_back(component);
        m_addComponent(component);
        component->recordParent(*this);
    }

    void BaseDevice::registerHandler(vrpn_MESSAGEHANDLER handler,
                                     void *userdata,
                                     RawMessageType const &msgType) {
        m_getConnection()->register_handler(msgType.get(), handler, userdata,
                                            m_getSender().get());
    }

    void BaseDevice::unregisterHandler(vrpn_MESSAGEHANDLER handler,
                                       void *userdata,
                                       RawMessageType const &msgType) {
        m_getConnection()->unregister_handler(msgType.get(), handler, userdata,
                                              m_getSender().get());
    }

    RawMessageType BaseDevice::registerMessageType(const char *msgString) {
        return RawMessageType(
            m_getConnection()->register_message_type(msgString));
    }

    RawSenderType BaseDevice::getSender() { return m_getSender(); }

    void BaseDevice::update() {
        for (auto const &component : m_components) {
            component->update();
        }
        m_update();
    }

    void BaseDevice::sendPending() {
        m_getConnection()->send_pending_reports();
    }

    void BaseDevice::m_addComponent(DeviceComponentPtr) {
        // default do-nothing
    }

    void BaseDevice::m_packMessage(size_t len, const char *buf,
                                   util::time::TimeValue const &timestamp,
                                   RawMessageType const &msgType,
                                   uint32_t classOfService) {
        struct timeval t;
        util::time::toStructTimeval(t, timestamp);
        auto ret = m_getConnection()->pack_message(
            static_cast<uint32_t>(len), t, msgType.get(), getSender().get(),
            buf, classOfService);
    }

    void BaseDevice::m_setConnection(vrpn_ConnectionPtr conn) { m_conn = conn; }

    vrpn_ConnectionPtr BaseDevice::m_getConnection() const { return m_conn; }
} // namespace common
} // namespace osvr