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
#include <osvr/Common/DirectionComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes

namespace osvr {
namespace common {

    namespace messages {
        class DirectionRecord::MessageSerialization {
          public:
            MessageSerialization(OSVR_DirectionState const &direction,
                                 OSVR_ChannelCount sensor)
                : m_direction(direction), m_sensor(sensor) {}

            MessageSerialization() {}

            template <typename T> void processMessage(T &p) {
                p(m_direction);
                p(m_sensor);
            }
            DirectionData getData() const {
                DirectionData ret;
                ret.sensor = m_sensor;
                ret.direction = m_direction;
                return ret;
            }

          private:
            OSVR_DirectionState m_direction;
            OSVR_ChannelCount m_sensor;
        };
        const char *DirectionRecord::identifier() {
            return "com.osvr.direction.directionrecord";
        }
    } // namespace messages

    shared_ptr<DirectionComponent>
    DirectionComponent::create(OSVR_ChannelCount numChan) {
        shared_ptr<DirectionComponent> ret(new DirectionComponent(numChan));
        return ret;
    }

    DirectionComponent::DirectionComponent(OSVR_ChannelCount numChan)
        : m_numSensor(numChan) {}

    DirectionComponent::~DirectionComponent() = default;

    void
    DirectionComponent::sendDirectionData(OSVR_DirectionState direction,
                                          OSVR_ChannelCount sensor,
                                          OSVR_TimeValue const &timestamp) {

        Buffer<> buf;
        messages::DirectionRecord::MessageSerialization msg(direction, sensor);
        serialize(buf, msg);

        m_getParent().packMessage(buf, directionRecord.getMessageType(),
                                  timestamp);
    }

    int VRPN_CALLBACK
    DirectionComponent::m_handleDirectionRecord(void *userdata,
                                                vrpn_HANDLERPARAM p) {
        auto self = static_cast<DirectionComponent *>(userdata);
        auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

        messages::DirectionRecord::MessageSerialization msg;
        deserialize(bufReader, msg);
        auto data = msg.getData();
        auto timestamp = util::time::fromStructTimeval(p.msg_time);

        for (auto const &cb : self->m_cb) {
            cb(data, timestamp);
        }
        return 0;
    }

    void
    DirectionComponent::registerDirectionHandler(DirectionHandler handler) {
        if (m_cb.empty()) {
            m_registerHandler(&DirectionComponent::m_handleDirectionRecord,
                              this, directionRecord.getMessageType());
        }
        m_cb.push_back(handler);
    }
    void DirectionComponent::m_parentSet() {
        m_getParent().registerMessageType(directionRecord);
    }

} // namespace common
} // namespace osvr
