/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_VRPNTrackerRouter_h_GUID_C1F1095D_9483_4DB0_7A7A_9F82837E6C9C
#define INCLUDED_VRPNTrackerRouter_h_GUID_C1F1095D_9483_4DB0_7A7A_9F82837E6C9C

// Internal Includes
#include "VRPNContext.h"
#include <osvr/Util/QuatlibInteropC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/Transform.h>

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    class VRPNTrackerRouter : public RouterEntry {
      public:
        VRPNTrackerRouter(common::ClientContext *ctx, vrpn_ConnectionPtr conn,
                          const char *src, boost::optional<int> sensor,
                          const char *dest, common::Transform const &t)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Tracker_Remote(src, conn.get())),
              m_transform(t), m_conn(conn) {
            m_remote->register_change_handler(this, &VRPNTrackerRouter::handle,
                                              sensor.get_value_or(-1));
            m_remote->shutup = true;
        }

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {
            VRPNTrackerRouter *self =
                static_cast<VRPNTrackerRouter *>(userdata);
            OSVR_PoseReport report;
            report.sensor = info.sensor;
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
            osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
            osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
            Eigen::Matrix4d pose = self->m_transform.transform(
                util::fromPose(report.pose).matrix());
            util::toPose(pose, report.pose);

            for (auto const &iface : self->getContext()->getInterfaces()) {
                if (iface->getPath() == self->getDest()) {
                    iface->triggerCallbacks(timestamp, report);
                }
            }

            /// @todo current heuristic for "do we have position data?" is
            /// "is our position non-zero?"
            if (util::vecMap(report.pose.translation) !=
                Eigen::Vector3d::Zero()) {
                OSVR_PositionReport positionReport;
                positionReport.sensor = info.sensor;
                positionReport.xyz = report.pose.translation;
                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, positionReport);
                    }
                }
            }

            /// @todo check to see if rotation is useful/provided
            {
                OSVR_OrientationReport oriReport;
                oriReport.sensor = info.sensor;
                oriReport.rotation = report.pose.rotation;
                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, oriReport);
                    }
                }
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        common::Transform m_transform;
        vrpn_ConnectionPtr m_conn;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNTrackerRouter_h_GUID_C1F1095D_9483_4DB0_7A7A_9F82837E6C9C
