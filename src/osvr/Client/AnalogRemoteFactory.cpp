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
#include "AnalogRemoteFactory.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/QuatlibInteropC.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/Transform.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/JSONTransformVisitor.h>
#include "PureClientContext.h"
#include "InterfaceTree.h"
#include <osvr/Util/ValueOrRange.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <vrpn_Analog.h>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    class VRPNAnalogHandler : public RemoteHandler {
      public:
        typedef util::ValueOrRange<int> RangeType;
        VRPNAnalogHandler(vrpn_ConnectionPtr const &conn, const char *src,
                          boost::optional<int> sensor,
                          common::InterfaceList &ifaces)
            : m_remote(new vrpn_Analog_Remote(src, conn.get())),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()) {
            m_remote->register_change_handler(this, &VRPNAnalogHandler::handle);
            OSVR_DEV_VERBOSE("Constructed an AnalogHandler for " << src);

            if (sensor.is_initialized()) {
                m_sensors.setValue(*sensor);
            }
        }
        virtual ~VRPNAnalogHandler() {
            m_remote->unregister_change_handler(this,
                                                &VRPNAnalogHandler::handle);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_ANALOGCB info) {
            auto self = static_cast<VRPNAnalogHandler *>(userdata);
            self->m_handle(info);
        }
        virtual void update() { m_remote->mainloop(); }

      private:
        void m_handle(vrpn_ANALOGCB const &info) {
            auto maxChannel = info.num_channel - 1;
            if (m_sensors.isValue() && maxChannel < m_sensors.getValue()) {
                // early out if we're looking for just a single channel number
                // that isn't in this report.
                return;
            }
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

            if (m_all) {
                if (m_sensors.empty()) {
                    m_sensors.setRangeMaxMin(maxChannel);
                } else {
                    m_sensors.extendRangeToMax(maxChannel);
                }
            }
            for (auto sensor : m_sensors.getIntersection(
                     RangeType::RangeZeroTo(maxChannel))) {
                OSVR_AnalogReport report;
                report.sensor = sensor;
                /// @todo handle transform?
                report.state = info.channel[report.sensor];
                for (auto &iface : m_interfaces) {
                    iface->triggerCallbacks(timestamp, report);
                }
            }
        }
        unique_ptr<vrpn_Analog_Remote> m_remote;
        common::InterfaceList &m_interfaces;
        bool m_all;
        RangeType m_sensors;
    };

    AnalogRemoteFactory::AnalogRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> AnalogRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &) {

        shared_ptr<RemoteHandler> ret;

        if (source.hasTransform()) {
            OSVR_DEV_VERBOSE(
                "Ignoring transform found on route for Analog data!");
        }

        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new VRPNAnalogHandler(m_conns.getConnection(devElt),
                                        devElt.getFullDeviceName().c_str(),
                                        source.getSensorNumber(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr