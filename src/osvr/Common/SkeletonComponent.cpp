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
#include <osvr/Common/SkeletonComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes

// Standard includes
// - none

namespace osvr {
namespace common {

    namespace messages {
        class SkeletonRecord::MessageSerialization {
          public:
            MessageSerialization(SkeletonNotification notification)
                : m_notification(notification) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_notification.sensor);
            }
            SkeletonNotification getNotification() const {
                SkeletonNotification ret;
                ret.sensor = m_notification.sensor;
                return ret;
            }

          private:
            SkeletonNotification m_notification;
        };
        const char *SkeletonRecord::identifier() {
            return "com.osvr.skeleton.skeletonrecord";
        }
    } // namespace messages

    shared_ptr<SkeletonComponent>
    SkeletonComponent::create(OSVR_ChannelCount numChan) {
        shared_ptr<SkeletonComponent> ret(new SkeletonComponent(numChan));
        return ret;
    }

    SkeletonComponent::SkeletonComponent(OSVR_ChannelCount numChan)
        : m_numSensor(numChan) {}

    void SkeletonComponent::sendNotification(OSVR_ChannelCount sensor,
                                             OSVR_TimeValue const &timestamp) {

        Buffer<> buf;
        SkeletonNotification notification;
        notification.sensor = sensor;
        messages::SkeletonRecord::MessageSerialization msg(notification);

        serialize(buf, msg);

        m_getParent().packMessage(buf, skeletonRecord.getMessageType(),
                                  timestamp);
    }

    int VRPN_CALLBACK SkeletonComponent::m_handleSkeletonRecord(
        void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<SkeletonComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::SkeletonRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getNotification();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb) {
            cb(data, timestamp);
        }
        return 0;
    }

    void SkeletonComponent::registerSkeletonHandler(SkeletonHandler handler) {
        if (m_cb.empty()) {
            m_registerHandler(&SkeletonComponent::m_handleSkeletonRecord, this,
                              skeletonRecord.getMessageType());
        }
        m_cb.push_back(handler);
    }
    void SkeletonComponent::m_parentSet() {
        m_getParent().registerMessageType(skeletonRecord);
    }

} // namespace common
} // namespace osvr