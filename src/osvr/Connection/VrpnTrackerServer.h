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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC
#define INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC

// Internal Includes
#include "DeviceConstructionData.h"
#include <osvr/Connection/TrackerServerInterface.h>
#include <osvr/Util/QuatlibInteropC.h>

// Library/third-party includes
#include <quat.h>
#include <vrpn_Tracker.h>

// Standard includes
// - none

namespace osvr {
namespace connection {
    class VrpnTrackerServer : public vrpn_Tracker,
                              public TrackerServerInterface {
      public:
        typedef vrpn_Tracker Base;
        VrpnTrackerServer(DeviceConstructionData &init)
            : vrpn_Tracker(init.getQualifiedName().c_str(), init.conn) {
            // Initialize data
            m_resetPos();
            m_resetQuat();

            m_resetVel();
            m_resetAccel();

            // Report interface out.
            init.obj.returnTrackerInterface(*this);
        }
        static const vrpn_uint32 CLASS_OF_SERVICE = vrpn_CONNECTION_LOW_LATENCY;

        virtual void sendReport(OSVR_PositionState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &tv) {
            m_resetQuat();
            osvrVec3ToQuatlib(Base::pos, &val);
            m_sendPose(sensor, tv);
        }

        virtual void sendReport(OSVR_OrientationState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &tv) {
            m_resetPos();
            osvrQuatToQuatlib(Base::d_quat, &val);
            m_sendPose(sensor, tv);
        }

        virtual void sendReport(OSVR_PoseState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &tv) {
            osvrVec3ToQuatlib(Base::pos, &(val.translation));
            osvrQuatToQuatlib(Base::d_quat, &(val.rotation));
            m_sendPose(sensor, tv);
        }

        void sendVelReport(OSVR_VelocityState const &val,
                           OSVR_ChannelCount sensor,
                           util::time::TimeValue const &tv) override {
            osvrVec3ToQuatlib(Base::vel, &(val.linearVelocity));
            osvrQuatToQuatlib(Base::vel_quat,
                              &(val.angularVelocity.incrementalRotation));
            Base::vel_quat_dt = val.angularVelocity.dt;
            m_sendVelocity(sensor, tv);
        }
        void sendVelReport(OSVR_LinearVelocityState const &val,
                           OSVR_ChannelCount sensor,
                           util::time::TimeValue const &tv) override {
            m_resetVel();

            osvrVec3ToQuatlib(Base::vel, &val);
            m_sendVelocity(sensor, tv);
        }
        void sendVelReport(OSVR_AngularVelocityState const &val,
                           OSVR_ChannelCount sensor,
                           util::time::TimeValue const &tv) override {
            m_resetVel();

            osvrQuatToQuatlib(Base::vel_quat, &(val.incrementalRotation));
            Base::vel_quat_dt = val.dt;
            m_sendVelocity(sensor, tv);
        }

        void sendAccelReport(OSVR_AccelerationState const &val,
                             OSVR_ChannelCount sensor,
                             util::time::TimeValue const &tv) override {
            osvrVec3ToQuatlib(Base::acc, &(val.linearAcceleration));
            osvrQuatToQuatlib(Base::acc_quat,
                              &(val.angularAcceleration.incrementalRotation));
            Base::acc_quat_dt = val.angularAcceleration.dt;
            m_sendAccel(sensor, tv);
        }
        void sendAccelReport(OSVR_LinearAccelerationState const &val,
                             OSVR_ChannelCount sensor,
                             util::time::TimeValue const &tv) override {
            m_resetAccel();

            osvrVec3ToQuatlib(Base::acc, &val);
            m_sendAccel(sensor, tv);
        }
        void sendAccelReport(OSVR_AngularAccelerationState const &val,
                             OSVR_ChannelCount sensor,
                             util::time::TimeValue const &tv) override {
            m_resetVel();

            osvrQuatToQuatlib(Base::acc_quat, &(val.incrementalRotation));
            Base::acc_quat_dt = val.dt;
            m_sendAccel(sensor, tv);
        }

      private:
        void m_resetVec3(vrpn_float64 vec[3]) {
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 0;
        }
        void m_resetPos() { m_resetVec3(pos); }
        void m_resetQuat(vrpn_float64 quat[4]) {
            quat[Q_W] = 1;
            quat[Q_X] = 0;
            quat[Q_Y] = 0;
            quat[Q_Z] = 0;
        }
        void m_resetQuat() { m_resetQuat(d_quat); }

        void m_resetVel() {
            m_resetVec3(Base::vel);
            m_resetQuat(Base::vel_quat);
            Base::vel_quat_dt = 0;
        }

        void m_resetAccel() {
            m_resetVec3(Base::acc);
            m_resetQuat(Base::acc_quat);
            Base::acc_quat_dt = 0;
        }

        void m_sendPose(OSVR_ChannelCount sensor,
                        util::time::TimeValue const &ts) {

            Base::d_sensor = sensor;
            util::time::toStructTimeval(Base::timestamp, ts);
            char msgbuf[1000];
            vrpn_int32 len = Base::encode_to(msgbuf);
            d_connection->pack_message(len, Base::timestamp,
                                       Base::position_m_id, Base::d_sender_id,
                                       msgbuf, CLASS_OF_SERVICE);
        }

        void m_sendVelocity(OSVR_ChannelCount sensor,
                            util::time::TimeValue const &ts) {

            Base::d_sensor = sensor;
            util::time::toStructTimeval(Base::timestamp, ts);
            char msgbuf[1000];
            vrpn_int32 len = Base::encode_vel_to(msgbuf);
            d_connection->pack_message(len, Base::timestamp,
                                       Base::velocity_m_id, Base::d_sender_id,
                                       msgbuf, CLASS_OF_SERVICE);
        }

        void m_sendAccel(OSVR_ChannelCount sensor,
                         util::time::TimeValue const &ts) {

            Base::d_sensor = sensor;
            util::time::toStructTimeval(Base::timestamp, ts);
            char msgbuf[1000];
            vrpn_int32 len = Base::encode_acc_to(msgbuf);
            d_connection->pack_message(len, Base::timestamp, Base::accel_m_id,
                                       Base::d_sender_id, msgbuf,
                                       CLASS_OF_SERVICE);
        }
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC
