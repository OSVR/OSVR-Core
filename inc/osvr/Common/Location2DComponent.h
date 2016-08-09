/** @file
    @brief Header

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

#ifndef INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF
#define INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF

// Internal Includes
#include <osvr/Common/Export.h>

#include <osvr/Common/DeviceComponent.h>
#include <osvr/Common/SerializationTags.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
#include <functional>
#include <tuple>
#include <vector>

namespace osvr {
namespace common {

    /// Just forward deserialized messages on to a simple list of handlers.
    template <typename MessageRegistrationType> class SimpleMessageHandler {
      public:
        SimpleMessageHandler(MessageRegistrationType &reg)
            : reg_(std::ref(reg)) {}

        using type = SimpleMessageHandler<MessageRegistrationType>;

        using MessageType = typename MessageRegistrationType::MessageType;

        using IndividualHandler = std::function<void(
            MessageType const &, util::time::TimeValue const &)>;

        /// @brief to pass to m_registerHandler()
        HandlerRegistrationArgs getRawHandlerRegistrationArgs() {
            return HandlerRegistrationArgs{&type::m_handleReportTrampoline,
                                           this, reg_.get().getMessageType()};
        }

        /// @brief Returns true exactly once, when you need to call
        /// m_registerHandler with getRawRegistrationArgs()
        bool registerSubordinateHandler(IndividualHandler &&handler) {
            bool ret = needsRegister_;
            needsRegister_ = false;
            m_cb.emplace_back(std::move(handler));
            return ret;
        }

      private:
        static int VRPN_CALLBACK m_handleReportTrampoline(void *userdata,
                                                          vrpn_HANDLERPARAM p) {
            static_cast<type *>(userdata)->m_handleReport(p);
            return 0;
        }
        void m_handleReport(vrpn_HANDLERPARAM const &p) {
            if (m_cb.empty()) {
                // early out if it's empty.
                return;
            }
            auto bufReader = readExternalBuffer(p.buffer, p.payload_len);

            MessageData data;
            serialization::deserializeRaw(bufReader, data);
            auto timestamp = util::time::fromStructTimeval(p.msg_time);

            for (auto const &cb : m_cb) {
                cb(data, timestamp);
            }
        }
        std::vector<IndividualHandler> m_cb;
        bool needsRegister_ = true;
        std::reference_wrapper<MessageRegistrationType> reg_;
    };

    using LocationData = OSVR_Location2DReport;

    namespace messages {
        class LocationRecord : public MessageRegistration<LocationRecord> {

          public:
            using MessageType = LocationData;
            static const char *identifier();
        };

    } // namespace messages

    /// @brief BaseDevice component
    class Location2DComponent : public DeviceComponent {
      public:
        /// @brief Factory method
        ///
        /// Required to ensure that allocation and deallocation stay on the same
        /// side of a DLL line.
        static OSVR_COMMON_EXPORT shared_ptr<Location2DComponent>
        create(OSVR_ChannelCount numSensor = 1);

        /// @brief Message from server to client, containing 2D location data.
        messages::LocationRecord locationRecord;

        OSVR_COMMON_EXPORT void
        sendLocationData(OSVR_Location2DState location,
                         OSVR_ChannelCount sensor,
                         OSVR_TimeValue const &timestamp);
        using LocationHandler =
            SimpleMessageHandler<messages::LocationRecord>::IndividualHandler;
        OSVR_COMMON_EXPORT void registerLocationHandler(LocationHandler cb);

      private:
        Location2DComponent(OSVR_ChannelCount numChan);
        void m_parentSet() override;

        OSVR_ChannelCount m_numSensor;
        SimpleMessageHandler<messages::LocationRecord> m_locationMsgs;
        bool m_gotOne;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_Location2DComponent_h_GUID_A831481F_0257_449A_6404_3B789BD89CEF
