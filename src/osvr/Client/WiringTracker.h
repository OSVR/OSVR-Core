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

#ifndef INCLUDED_WiringTracker_h_GUID_C473E294_CC7C_49A2_C03F_B47458E22EDB
#define INCLUDED_WiringTracker_h_GUID_C473E294_CC7C_49A2_C03F_B47458E22EDB

// Internal Includes
#include "VRPNConnectionCollection.h"
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/QuatlibInteropC.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/Transform.h>
#include <osvr/Common/DecomposeOriginalSource.h>
#include "PureClientContext.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    class VRPNTrackerHandler : public Updatable {
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
                           ClientInterfacePtr const &iface)
            : m_remote(new vrpn_Tracker_Remote(src, conn.get())),
              m_transform(t), m_iface(iface), m_opts(options) {
            m_remote->register_change_handler(this, &VRPNTrackerHandler::handle,
                                              sensor.get_value_or(-1));
            OSVR_DEV_VERBOSE("Constructed a TrackerHandler for "
                             << src << " sensor " << sensor.get_value_or(-1));
#if 0
            m_remote->shutup = true;
#endif
        }

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {

            auto self = static_cast<VRPNTrackerHandler *>(userdata);

            ClientInterfacePtr iface(self->m_iface.lock());
            if (iface) {
                self->m_handle(iface, info);
            }
        }
        virtual void update() { m_remote->mainloop(); }

      private:
        void m_handle(ClientInterfacePtr iface, vrpn_TRACKERCB const &info) {
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
                iface->triggerCallbacks(timestamp, report);
            }

            if (m_opts.reportPosition) {
                OSVR_PositionReport positionReport;
                positionReport.sensor = info.sensor;
                positionReport.xyz = report.pose.translation;
                iface->triggerCallbacks(timestamp, positionReport);
            }

            if (m_opts.reportOrientation) {
                OSVR_OrientationReport oriReport;
                oriReport.sensor = info.sensor;
                oriReport.rotation = report.pose.rotation;
                iface->triggerCallbacks(timestamp, oriReport);
            }
        }
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        common::Transform m_transform;
        ClientInterfaceWeakPtr m_iface;
        Options m_opts;
    };

    class WiringTracker {
      public:
        WiringTracker(VRPNConnectionCollection const &conns) : m_conns(conns) {}

        template <typename T>
        static void createAndAddFactory(VRPNConnectionCollection const &conns,
                                        T &factory) {
            factory.addFactory("tracker", WiringTracker(conns));
            factory.addFactory("pose", WiringTracker(conns));
            factory.addFactory("position", WiringTracker(conns));
            factory.addFactory("orientation", WiringTracker(conns));
        }

        shared_ptr<Updatable> operator()(common::PathNode &node,
                                         ClientInterfacePtr const &iface) {
            common::Transform identityXform{};
            return (*this)(node, iface, identityXform);
        }

        shared_ptr<Updatable> operator()(common::PathNode &node,
                                         ClientInterfacePtr const &iface,
                                         common::Transform const &t) {

            shared_ptr<Updatable> ret;
            common::DecomposeOriginalSource decomp{node};

            if (!decomp.gotDeviceAndInterface()) {
                OSVR_DEV_VERBOSE("Got into a tracker wiring factory without "
                                 "having a device and interface node - should "
                                 "not happen!");
                BOOST_ASSERT_MSG(decomp.gotDeviceAndInterface(),
                                 "Got into a tracker wiring factory without "
                                 "having a device and interface node - should "
                                 "not happen!");
                return ret;
            }

            /// @todo set this struct correctly from the descriptor, or perhaps
            /// the path?
            VRPNTrackerHandler::Options opts;

            auto interfaceType = decomp.getInterfaceName();
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

            auto const &devElt = decomp.getDeviceElement();
#if 0
            ret = make_shared<VRPNTrackerHandler>(
                m_conns.getConnection(devElt),
                devElt.getFullDeviceName().c_str(), opts, t,
                decomp.getSensorNumber(), iface);
#endif
            ret.reset(new VRPNTrackerHandler(m_conns.getConnection(devElt),
                                             devElt.getFullDeviceName().c_str(),
                                             opts, t, decomp.getSensorNumber(),
                                             iface));
            return ret;
        }

      private:
        VRPNConnectionCollection m_conns;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_WiringTracker_h_GUID_C473E294_CC7C_49A2_C03F_B47458E22EDB
