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
#include "TrackerRemoteFactory.h"
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
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    class VRPNTrackerHandler : public RemoteHandler {
      public:
        struct Options {
            Options()
                : reportPose(false), reportPosition(false),
                  reportOrientation(false) {}
            bool reportPose;
            bool reportPosition;
            bool reportOrientation;
        };
        VRPNTrackerHandler(vrpn_ConnectionPtr const &conn, const char *src,
                           Options const &options, common::Transform const &t,
                           boost::optional<int> sensor,
                           common::InterfaceList &ifaces)
            : m_remote(new vrpn_Tracker_Remote(src, conn.get())),
              m_transform(t), m_interfaces(ifaces), m_opts(options),
              m_sensor(sensor) {
            m_remote->register_change_handler(this, &VRPNTrackerHandler::handle,
                                              sensor.get_value_or(-1));
            OSVR_DEV_VERBOSE("Constructed a TrackerHandler for "
                             << src << " sensor " << sensor.get_value_or(-1));
        }
        virtual ~VRPNTrackerHandler() {
            m_remote->unregister_change_handler(
                this, &VRPNTrackerHandler::handle, m_sensor.get_value_or(-1));
        }

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {
            auto self = static_cast<VRPNTrackerHandler *>(userdata);
            self->m_handle(info);
        }
        virtual void update() { m_remote->mainloop(); }

      private:
        void m_handle(vrpn_TRACKERCB const &info) {
            OSVR_PoseReport report;
            report.sensor = info.sensor;
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
            osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
            osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
            Eigen::Matrix4d pose =
                m_transform.transform(util::fromPose(report.pose).matrix());
            util::toPose(pose, report.pose);

            if (m_opts.reportPose) {
                for (auto &iface : m_interfaces) {
                    iface->triggerCallbacks(timestamp, report);
                }
            }

            if (m_opts.reportPosition) {
                OSVR_PositionReport positionReport;
                positionReport.sensor = info.sensor;
                positionReport.xyz = report.pose.translation;
                for (auto &iface : m_interfaces) {
                    iface->triggerCallbacks(timestamp, positionReport);
                }
            }

            if (m_opts.reportOrientation) {
                OSVR_OrientationReport oriReport;
                oriReport.sensor = info.sensor;
                oriReport.rotation = report.pose.rotation;

                for (auto &iface : m_interfaces) {
                    iface->triggerCallbacks(timestamp, oriReport);
                }
            }
        }
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        common::Transform m_transform;
        common::InterfaceList &m_interfaces;
        Options m_opts;
        boost::optional<int> m_sensor;
    };

    TrackerRemoteFactory::TrackerRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> TrackerRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &) {

        shared_ptr<RemoteHandler> ret;

        /// @todo set this struct correctly from the descriptor, or perhaps
        /// the path?
        VRPNTrackerHandler::Options opts;

        auto interfaceType = source.getInterfaceName();
        if ("position" == interfaceType) {
            opts.reportPosition = true;
        } else if ("orientation" == interfaceType) {
            opts.reportOrientation = true;
        } else {
            /// pose and tracker both imply full reports (?)
            opts.reportOrientation = true;
            opts.reportPosition = true;
            opts.reportPose = true;
        }

        auto const &devElt = source.getDeviceElement();

        common::Transform xform{};
        if (source.hasTransform()) {
            common::JSONTransformVisitor xformParse(source.getTransformJson());
            xform = xformParse.getTransform();
        }

        /// @todo find out why make_shared causes a crash here
        ret.reset(new VRPNTrackerHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName().c_str(),
            opts, xform, source.getSensorNumber(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr