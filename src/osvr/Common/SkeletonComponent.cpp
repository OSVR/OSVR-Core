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
#include <osvr/Common/JSONSerializationTags.h>
#include <osvr/Common/CommonComponent.h>
#include <osvr/Connection/Connection.h>

// Library/third-party includes
#include <json/reader.h>

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

        class SkeletonSpecRecord::MessageSerialization {
          public:
            MessageSerialization(SkeletonSpec articSpec)
                : m_articSpec(articSpec) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_articSpec.spec);
            }
            SkeletonSpec getNotification() const {
                SkeletonSpec ret;
                ret.spec = m_articSpec.spec;
                return ret;
            }

          private:
            SkeletonSpec m_articSpec;
        };
        const char *SkeletonSpecRecord::identifier() {
            return "com.osvr.skeleton.skeletonspecrecord";
        }
    } // namespace messages

    shared_ptr<SkeletonComponent>
      SkeletonComponent::create(std::string const &jsonSpec, OSVR_ChannelCount numChan) {
        shared_ptr<SkeletonComponent> ret(new SkeletonComponent(jsonSpec, numChan));
        return ret;
    }

    SkeletonComponent::SkeletonComponent(std::string const &jsonSpec, OSVR_ChannelCount numChan)
        : m_numSensor(numChan), m_spec(jsonSpec) {}

    OSVR_ReturnCode
    SkeletonComponent::setArticulationSpec(std::string const &jsonDescriptor) {
        /// @todo add validation to make sure that articulation spec is provided
        m_spec = jsonDescriptor;
        return OSVR_RETURN_SUCCESS;
    }

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
    void SkeletonComponent::sendArticulationSpec(std::string const &jsonSpec) {

        Buffer<> buf;
        SkeletonSpec articSpec;
        Json::Reader reader;
        Json::Value spec;
        reader.parse(jsonSpec, spec);
        articSpec.spec = spec["articulationSpec"];
        /// @todo do we need to verify that it read the spec correctly?
        messages::SkeletonSpecRecord::MessageSerialization msg(articSpec);

        serialize(buf, msg);
 
        OSVR_TimeValue timestamp;
        osvrTimeValueGetNow(&timestamp);
        m_getParent().packMessage(buf, skeletonSpecRecord.getMessageType(),
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

    int VRPN_CALLBACK SkeletonComponent::m_handleSkeletonSpecRecord(
        void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<SkeletonComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::SkeletonSpecRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getNotification();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb_spec) {
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
    void SkeletonComponent::registerSkeletonSpecHandler(
        SkeletonSpecHandler handler) {
        if (m_cb_spec.empty()) {
            m_registerHandler(&SkeletonComponent::m_handleSkeletonSpecRecord,
                              this, skeletonSpecRecord.getMessageType());
                              
        }
        m_cb_spec.push_back(handler);
    }
    void SkeletonComponent::m_parentSet() {
        // add a ping handler to re-send skeleton articulation spec everytime
        // the new
        // connection(ping) occurs
        m_commonComponent =
            m_getParent().addComponent(osvr::common::CommonComponent::create());
        OSVR_TimeValue now;
        osvrTimeValueGetNow(&now);
        m_commonComponent->registerPingHandler(
            [&] { sendArticulationSpec(m_spec); });

        m_getParent().registerMessageType(skeletonRecord);
        m_getParent().registerMessageType(skeletonSpecRecord);
    }

} // namespace common
} // namespace osvr