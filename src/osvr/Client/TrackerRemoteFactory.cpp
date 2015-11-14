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
#include "RemoteHandlerInternals.h"
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
#include <osvr/Common/Tracing.h>
#include <osvr/Common/TrackerSensorInfo.h>

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
// - none

namespace ei = osvr::util::eigen_interop;

namespace osvr {
namespace client {
    class VRPNTrackerHandler : public RemoteHandler {
      public:
        struct Options {
            bool reportPose = false;
            bool reportPosition = false;
            bool reportOrientation = false;
        };
        VRPNTrackerHandler(vrpn_ConnectionPtr const &conn, const char *src,
                           Options const &options,
                           common::TrackerSensorInfo const &info,
                           common::Transform const &t,
                           boost::optional<int> sensor,
                           common::InterfaceList &ifaces,
                           common::ClientContext &ctx)
            : m_remote(new vrpn_Tracker_Remote(src, conn.get())),
              m_transform(t), m_ctx(ctx), m_internals(ifaces), m_opts(options),
              m_info(info), m_sensor(sensor) {
            if (m_info.reportsPosition || m_info.reportsOrientation) {
                m_remote->register_change_handler(this,
                                                  &VRPNTrackerHandler::handle,
                                                  m_sensor.get_value_or(-1));
            }
            if (m_info.reportsLinearVelocity || m_info.reportsAngularVelocity) {
                m_remote->register_change_handler(
                    this, &VRPNTrackerHandler::handleVel,
                    m_sensor.get_value_or(-1));
            }
            if (m_info.reportsLinearAcceleration ||
                m_info.reportsAngularAcceleration) {
                m_remote->register_change_handler(
                    this, &VRPNTrackerHandler::handleAccel,
                    m_sensor.get_value_or(-1));
            }
            OSVR_DEV_VERBOSE("Constructed a TrackerHandler for "
                             << src << " sensor " << m_sensor.get_value_or(-1));
        }
        virtual ~VRPNTrackerHandler() {
            if (m_info.reportsPosition || m_info.reportsOrientation) {
                m_remote->unregister_change_handler(this,
                                                    &VRPNTrackerHandler::handle,
                                                    m_sensor.get_value_or(-1));
            }
            if (m_info.reportsLinearVelocity || m_info.reportsAngularVelocity) {
                m_remote->unregister_change_handler(
                    this, &VRPNTrackerHandler::handleVel,
                    m_sensor.get_value_or(-1));
            }
            if (m_info.reportsLinearAcceleration ||
                m_info.reportsAngularAcceleration) {
                m_remote->unregister_change_handler(
                    this, &VRPNTrackerHandler::handleAccel,
                    m_sensor.get_value_or(-1));
            }
        }

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        common::Transform getCurrentTransform() const {
            auto ret = m_transform;
            ret.transform(m_ctx.getRoomToWorldTransform());
            return ret;
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {
            auto self = static_cast<VRPNTrackerHandler *>(userdata);
            self->m_handle(info);
        }
        static void VRPN_CALLBACK handleVel(void *userdata,
                                            vrpn_TRACKERVELCB info) {
            auto self = static_cast<VRPNTrackerHandler *>(userdata);
            self->m_handle(info);
        }
        static void VRPN_CALLBACK handleAccel(void *userdata,
                                              vrpn_TRACKERACCCB info) {
            auto self = static_cast<VRPNTrackerHandler *>(userdata);
            self->m_handle(info);
        }
        virtual void update() { m_remote->mainloop(); }

      private:
        /// Pass pose messages on to the client
        void m_handle(vrpn_TRACKERCB const &info) {
            common::tracing::markNewTrackerData();
            OSVR_PoseReport report;
            report.sensor = info.sensor;
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
            osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
            osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
            auto xform = getCurrentTransform();
            ei::map(report.pose) =
                xform.transform(ei::map(report.pose).matrix());

            if (m_opts.reportPose) {
                m_internals.setStateAndTriggerCallbacks(timestamp, report);
            }

            if (m_opts.reportPosition) {
                OSVR_PositionReport positionReport;
                positionReport.sensor = info.sensor;
                positionReport.xyz = report.pose.translation;

                m_internals.setStateAndTriggerCallbacks(timestamp,
                                                        positionReport);
            }

            if (m_opts.reportOrientation) {
                OSVR_OrientationReport oriReport;
                oriReport.sensor = info.sensor;
                oriReport.rotation = report.pose.rotation;

                m_internals.setStateAndTriggerCallbacks(timestamp, oriReport);
            }
        }

        /// Pass velocity messages on to the client
        void m_handle(vrpn_TRACKERVELCB const &info) {
            /// @todo should we be marking a trace event here?
            // common::tracing::markNewTrackerData();

            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

            OSVR_VelocityReport overallReport;
            overallReport.sensor = info.sensor;
            auto xform = getCurrentTransform();

            overallReport.state.linearVelocityValid =
                m_info.reportsLinearVelocity;
            if (m_info.reportsLinearVelocity) {
                OSVR_LinearVelocityState vel;
                osvrVec3FromQuatlib(&(vel), info.vel);

                ei::map(vel) = xform.transformLinear(ei::map(vel));

                overallReport.state.linearVelocity = vel;
                OSVR_LinearVelocityReport report;
                report.sensor = info.sensor;
                report.state = vel;
                m_internals.setStateAndTriggerCallbacks(timestamp, report);
            }

            overallReport.state.angularVelocityValid =
                m_info.reportsAngularVelocity;
            if (m_info.reportsAngularVelocity) {
                OSVR_AngularVelocityState state;
                osvrQuatFromQuatlib(&(state.incrementalRotation),
                                    info.vel_quat);
                state.dt = info.vel_quat_dt;

                ei::map(state.incrementalRotation) =
                    xform.transformLinear(ei::map(state.incrementalRotation));

                overallReport.state.angularVelocity = state;
                OSVR_AngularVelocityReport report;
                report.sensor = info.sensor;
                report.state = state;
                m_internals.setStateAndTriggerCallbacks(timestamp, report);
            }

            m_internals.setStateAndTriggerCallbacks(timestamp, overallReport);
        }

        /// Pass acceleration messages on to the client
        void m_handle(vrpn_TRACKERACCCB const &info) {
            /// @todo should we be marking a trace event here?
            // common::tracing::markNewTrackerData();
            OSVR_TimeValue timestamp;
            osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

            OSVR_AccelerationReport overallReport;
            overallReport.sensor = info.sensor;

            auto xform = getCurrentTransform();

            overallReport.state.linearAccelerationValid =
                m_info.reportsLinearAcceleration;
            if (m_info.reportsLinearAcceleration) {
                OSVR_LinearAccelerationState accel;
                osvrVec3FromQuatlib(&(accel), info.acc);

                ei::map(accel) = xform.transformLinear(ei::map(accel));

                overallReport.state.linearAcceleration = accel;
                OSVR_LinearAccelerationReport report;
                report.sensor = info.sensor;
                report.state = accel;
                m_internals.setStateAndTriggerCallbacks(timestamp, report);
            }
            overallReport.state.angularAccelerationValid =
                m_info.reportsAngularAcceleration;
            if (m_info.reportsAngularAcceleration) {

                OSVR_AngularAccelerationState state;
                osvrQuatFromQuatlib(&(state.incrementalRotation),
                                    info.acc_quat);
                state.dt = info.acc_quat_dt;

                ei::map(state.incrementalRotation) =
                    xform.transformLinear(ei::map(state.incrementalRotation));

                overallReport.state.angularAcceleration = state;
                OSVR_AngularAccelerationReport report;
                report.sensor = info.sensor;
                report.state = state;
                m_internals.setStateAndTriggerCallbacks(timestamp, report);
            }

            m_internals.setStateAndTriggerCallbacks(timestamp, overallReport);
        }
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        common::Transform m_transform;
        common::ClientContext &m_ctx;
        RemoteHandlerInternals m_internals;
        Options m_opts;
        common::TrackerSensorInfo m_info;
        boost::optional<int> m_sensor;
    };

    TrackerRemoteFactory::TrackerRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> TrackerRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &ctx) {

        shared_ptr<RemoteHandler> ret;

        auto info = common::getTrackerSensorInfo(source);

        VRPNTrackerHandler::Options opts;
        /// @todo right now always reporting pose if we report either position
        /// or orientation as a backward-compatibility move, since we did so
        /// before, at least until we have a report like pose with validity
        /// bools in it.
        opts.reportPose = info.reportsPosition || info.reportsOrientation;
        opts.reportPosition = info.reportsPosition;
        opts.reportOrientation = info.reportsOrientation;

        auto const &devElt = source.getDeviceElement();

        common::Transform xform{};
        if (source.hasTransform()) {
            common::JSONTransformVisitor xformParse(source.getTransformJson());
            xform = xformParse.getTransform();
        }

        /// @todo find out why make_shared causes a crash here
        ret.reset(new VRPNTrackerHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName().c_str(),
            opts, info, xform, source.getSensorNumber(), ifaces, ctx));
        return ret;
    }

} // namespace client
} // namespace osvr
