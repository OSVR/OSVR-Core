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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Common/LocomotionComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    namespace messages {
        class NaviVelocityRecord::MessageSerialization {
          public:
            MessageSerialization(OSVR_NaviVelocityState const &state,
                                 OSVR_ChannelCount sensor)
                : m_naviVelState(state), m_sensor(sensor) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_naviVelState);
                p(m_sensor);
            }
            NaviVelocityData getData() const {
                NaviVelocityData ret;
                ret.sensor = m_sensor;
                ret.naviVelState = m_naviVelState;
                return ret;
            }

          private:
            OSVR_NaviVelocityState m_naviVelState;
            OSVR_ChannelCount m_sensor;
        };
        const char *NaviVelocityRecord::identifier() {
            return "com.osvr.locomotion.navivelocityrecord";
        }

        class NaviPositionRecord::MessageSerialization {
          public:
            MessageSerialization(OSVR_NaviPositionState const &state,
                                 OSVR_ChannelCount sensor)
                : m_naviPosnState(state), m_sensor(sensor) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_naviPosnState);
                p(m_sensor);
            }
            NaviPositionData getData() const {
                NaviPositionData ret;
                ret.sensor = m_sensor;
                ret.naviPosnState = m_naviPosnState;
                return ret;
            }

          private:
            OSVR_NaviPositionState m_naviPosnState;
            OSVR_ChannelCount m_sensor;
        };
        const char *NaviPositionRecord::identifier() {
            return "com.osvr.locomotion.navipositionrecord";
        }

    } // namespace messages

    shared_ptr<LocomotionComponent> LocomotionComponent::create() {
        shared_ptr<LocomotionComponent> ret(new LocomotionComponent());
        return ret;
    }

    LocomotionComponent::LocomotionComponent() {}

    void LocomotionComponent::sendNaviVelocityData(
        OSVR_NaviVelocityState naviVelocityState, OSVR_ChannelCount sensor,
        OSVR_TimeValue const &timestamp) {

        Buffer<> buf;

        messages::NaviVelocityRecord::MessageSerialization msg(
            naviVelocityState, sensor);

        serialize(buf, msg);
        m_getParent().packMessage(buf, naviVelRecord.getMessageType(),
                                  timestamp);
    }

    void LocomotionComponent::sendNaviPositionData(
        OSVR_NaviPositionState naviPositionState, OSVR_ChannelCount sensor,
        OSVR_TimeValue const &timestamp) {

        Buffer<> buf;

        messages::NaviPositionRecord::MessageSerialization msg(
            naviPositionState, sensor);
        serialize(buf, msg);

        m_getParent().packMessage(buf, naviPosnRecord.getMessageType(),
                                  timestamp);
    }

    int VRPN_CALLBACK
    LocomotionComponent::m_handleNaviVelocityRecord(void *userdata,
                                                    vrpn_HANDLERPARAM p) {
        auto self = static_cast<LocomotionComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::NaviVelocityRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb_vel) {
            cb(data, timestamp);
        }
        return 0;
    }

    int VRPN_CALLBACK
    LocomotionComponent::m_handleNaviPositionRecord(void *userdata,
                                                    vrpn_HANDLERPARAM p) {
        auto self = static_cast<LocomotionComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::NaviPositionRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb_posn) {
            cb(data, timestamp);
        }
        return 0;
    }

    void LocomotionComponent::registerNaviVelocityHandler(
        NaviVelocityHandler handler) {
        if (m_cb_vel.empty()) {
            m_registerHandler(&LocomotionComponent::m_handleNaviVelocityRecord,
                              this, naviVelRecord.getMessageType());
        }
        m_cb_vel.push_back(handler);
    }
    void LocomotionComponent::registerNaviPositionHandler(
        NaviPositionHandler handler) {
        if (m_cb_posn.empty()) {
            m_registerHandler(&LocomotionComponent::m_handleNaviPositionRecord,
                              this, naviPosnRecord.getMessageType());
        }
        m_cb_posn.push_back(handler);
    }

    void LocomotionComponent::m_parentSet() {

        m_getParent().registerMessageType(naviVelRecord);
        m_getParent().registerMessageType(naviPosnRecord);
    }

} // namespace common
} // namespace osvr
