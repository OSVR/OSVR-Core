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
#include <osvr/Common/GestureComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/PredefinedGesturesXMacroC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    namespace messages {
        class GestureRecord::MessageSerialization {
          public:
            MessageSerialization(OSVR_GestureState const &state,
                                 OSVR_GestureID gestureID,
                                 OSVR_ChannelCount sensor)
                : m_gestureState(state), m_gestureID(gestureID),
                  m_sensor(sensor) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_gestureState);
                p(m_gestureID);
                p(m_sensor);
            }
            GestureData getData() const {
                GestureData ret;
                ret.sensor = m_sensor;
                ret.gestureID = m_gestureID;
                ret.gestureState = m_gestureState;
                return ret;
            }

          private:
            OSVR_GestureState m_gestureState;
            util::StringID m_gestureID;
            OSVR_ChannelCount m_sensor;
        };
        const char *GestureRecord::identifier() {
            return "org.osvr.gesture.gesturerecord";
        }

    } // namespace messages

    shared_ptr<GestureComponent>
    GestureComponent::create(common::SystemComponent &systemComponent) {
        shared_ptr<GestureComponent> ret(new GestureComponent(systemComponent));
        return ret;
    }

    GestureComponent::GestureComponent(common::SystemComponent &systemComponent)
        : m_systemComponent(systemComponent),
          m_gestureNameMap(m_systemComponent.getGestureMap()) {

        // populate the gesture map
#define OSVR_X(GESTURE) m_getStringID(GESTURE);
        OSVR_GESTURE_X()
#undef OSVR_X

#if 0
        m_getStringID(OSVR_GESTURE_SWIPE_LEFT);
        m_getStringID(OSVR_GESTURE_SWIPE_RIGHT);
        m_getStringID(OSVR_GESTURE_SCROLL_UP);
        m_getStringID(OSVR_GESTURE_SCROLL_DOWN);
        m_getStringID(OSVR_GESTURE_SINGLE_TAP);
        m_getStringID(OSVR_GESTURE_DOUBLE_TAP);
        m_getStringID(OSVR_GESTURE_PINCH);
        m_getStringID(OSVR_GESTURE_FINGER_SPREAD);
        m_getStringID(OSVR_GESTURE_CIRCLE);
        m_getStringID(OSVR_GESTURE_LONG_PRESS);
        m_getStringID(OSVR_GESTURE_OPEN_HAND);
        m_getStringID(OSVR_GESTURE_CLOSED_HAND);
#endif

        // send out an updated map
        m_systemComponent.sendGestureMap();
    }

    void GestureComponent::sendGestureData(OSVR_GestureState gestureState,
                                           OSVR_GestureID gestureID,
                                           OSVR_ChannelCount sensor,
                                           OSVR_TimeValue const &timestamp) {

        Buffer<> buf;

        messages::GestureRecord::MessageSerialization msg(gestureState,
                                                          gestureID, sensor);
        serialize(buf, msg);

        m_getParent().packMessage(buf, gestureRecord.getMessageType(),
                                  timestamp);
    }

    int VRPN_CALLBACK GestureComponent::m_handleGestureRecord(
        void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<GestureComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::GestureRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb) {
            cb(data, timestamp);
        }
        return 0;
    }

    util::StringID GestureComponent::m_getStringID(std::string const &str) {
        return m_gestureNameMap->map.getStringID(str);
    }

    void GestureComponent::registerGestureHandler(GestureHandler handler) {
        if (m_cb.empty()) {
            m_registerHandler(&GestureComponent::m_handleGestureRecord, this,
                              gestureRecord.getMessageType());
        }
        m_cb.push_back(handler);
    }

    void GestureComponent::m_parentSet() {
        m_getParent().registerMessageType(gestureRecord);
    }

    OSVR_GestureID GestureComponent::getGestureID(const char *gestureName) {

        OSVR_GestureID gestureID = m_getStringID(gestureName).value();

        // if we just inserted new gesture ID then send gesture map
        if (m_gestureNameMap->map.isModified()) {
            m_systemComponent.sendGestureMap();
        }
        return gestureID;
    }

} // namespace common
} // namespace osvr
