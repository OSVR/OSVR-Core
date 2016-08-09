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
#include <osvr/Common/Location2DComponent.h>

#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Buffer.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/SerializationTraits.h>

// Library/third-party includes

namespace osvr {
namespace common {
    namespace serialization {
        template <>
        struct SimpleStructSerialization<OSVR_Location2DReport>
            : SimpleStructSerializationBase {
            template <typename F, typename T> static void apply(F &f, T &val) {
                f(val.location);
                f(val.sensor);
            }
        };
    } // namespace serialization
    namespace messages {
        const char *LocationRecord::identifier() {
            return "com.osvr.location2D.locationrecord";
        }
    } // namespace messages

    shared_ptr<Location2DComponent>
    Location2DComponent::create(OSVR_ChannelCount numChan) {
        shared_ptr<Location2DComponent> ret(new Location2DComponent(numChan));
        return ret;
    }

    Location2DComponent::Location2DComponent(OSVR_ChannelCount numChan)
        : locationRecord(), m_numSensor(numChan),
          m_locationMsgs(locationRecord) {}

    void
    Location2DComponent::sendLocationData(OSVR_Location2DState location,
                                          OSVR_ChannelCount sensor,
                                          OSVR_TimeValue const &timestamp) {
        Buffer<> buf;
        LocationData msg{sensor, location};
        serialization::serializeRaw(buf, msg);

        m_getParent().packMessage(buf, locationRecord.getMessageType(),
                                  timestamp);
    }

    void Location2DComponent::registerLocationHandler(LocationHandler handler) {
        if (m_locationMsgs.registerSubordinateHandler(std::move(handler))) {
            m_registerHandler(m_locationMsgs.getRawHandlerRegistrationArgs());
        }
    }
    void Location2DComponent::m_parentSet() {
        m_getParent().registerMessageType(locationRecord);
    }

} // namespace common
} // namespace osvr