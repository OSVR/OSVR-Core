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
#include "ButtonRemoteFactory.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/OriginalSource.h>
#include "InterfaceTree.h"
#include <osvr/Util/ValueOrRange.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <vrpn_Button.h>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    class VRPNButtonHandler : public RemoteHandler {
      public:
        typedef util::ValueOrRange<int> RangeType;
        VRPNButtonHandler(vrpn_ConnectionPtr const &conn, const char *src,
                          boost::optional<int> sensor,
                          common::InterfaceList &ifaces)
            : m_remote(new vrpn_Button_Remote(src, conn.get())),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()) {
            m_remote->register_change_handler(this, &VRPNButtonHandler::handle);
            m_remote->register_states_handler(
                this, &VRPNButtonHandler::handle_states);
            OSVR_DEV_VERBOSE("Constructed a ButtonHandler for " << src);

            if (sensor.is_initialized()) {
                m_sensors.setValue(*sensor);
            }
        }
        virtual ~VRPNButtonHandler() {
            m_remote->unregister_change_handler(this,
                                                &VRPNButtonHandler::handle);
            m_remote->unregister_states_handler(
                this, &VRPNButtonHandler::handle_states);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_BUTTONCB info) {
            auto self = static_cast<VRPNButtonHandler *>(userdata);
            self->m_handle(info);
        }

        static void VRPN_CALLBACK
        handle_states(void *userdata, vrpn_BUTTONSTATESCB info) {
            auto self = static_cast<VRPNButtonHandler *>(userdata);
            self->m_handle(info);
        }
        virtual void update() { m_remote->mainloop(); }

      private:
        void m_handle(vrpn_BUTTONCB const &info) {
            if (!m_all && !m_sensors.contains(info.button)) {
                return;
            }

            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

            m_report(timestamp, info.button, info.state);
        }
        void m_handle(vrpn_BUTTONSTATESCB const &info) {
            auto maxChannel = info.num_buttons - 1;
            if (!m_all &&
                m_sensors.getIntersection(RangeType::RangeZeroTo(maxChannel))
                    .empty()) {
                // early out if we're looking for just a single channel number
                // that isn't in this report.
                return;
            }
            if (m_all) {
                if (m_sensors.empty()) {
                    m_sensors.setRangeMaxMin(maxChannel);
                } else {
                    m_sensors.extendRangeToMax(maxChannel);
                }
            }
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

            for (auto sensor : m_sensors.getIntersection(
                     RangeType::RangeZeroTo(maxChannel))) {
                m_report(timestamp, sensor, info.states[sensor]);
            }
        }
        void m_report(OSVR_TimeValue const &timestamp, int32_t sensor,
                      vrpn_int32 state) {
            OSVR_ButtonReport report;
            report.sensor = sensor;
            report.state = static_cast<uint8_t>(state);
            for (auto &iface : m_interfaces) {
                iface->triggerCallbacks(timestamp, report);
            }
        }
        unique_ptr<vrpn_Button_Remote> m_remote;
        common::InterfaceList &m_interfaces;
        bool m_all;
        RangeType m_sensors;
    };

    ButtonRemoteFactory::ButtonRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> ButtonRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &) {

        shared_ptr<RemoteHandler> ret;

        if (source.hasTransform()) {
            OSVR_DEV_VERBOSE(
                "Ignoring transform found on route for Button data!");
        }

        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new VRPNButtonHandler(m_conns.getConnection(devElt),
                                        devElt.getFullDeviceName().c_str(),
                                        source.getSensorNumber(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr