/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC
#define INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC

// Internal Includes
#include <osvr/Connection/DeviceInitObject.h>
#include <osvr/Connection/TrackerServerInterface.h>
#include <osvr/Util/QuatlibInteropC.h>

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <quat.h>

// Standard includes
// - none

namespace osvr {
namespace connection {
    class VrpnTrackerServer : public vrpn_Tracker,
                              public TrackerServerInterface {
      public:
        typedef vrpn_Tracker Base;
        VrpnTrackerServer(DeviceInitObject &init, vrpn_Connection *conn)
            : vrpn_Tracker(init.getQualifiedName().c_str(), conn) {
            // Initialize data
            m_resetPos();
            m_resetQuat();
        }
        static const vrpn_uint32 CLASS_OF_SERVICE = vrpn_CONNECTION_LOW_LATENCY;

        virtual void sendReport(OSVR_PositionState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) {
            m_resetQuat();
            osvrVec3ToQuatlib(Base::pos, &val);
            m_sendPose(chan, timestamp);
        }

        virtual void sendReport(OSVR_OrientationState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) {
            m_resetPos();
            osvrQuatToQuatlib(Base::d_quat, &val);
            m_sendPose(chan, timestamp);
        }

        virtual void sendReport(OSVR_PoseState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) {
            osvrQuatToQuatlib(Base::d_quat, &(val.rotation));
            osvrVec3ToQuatlib(Base::pos, &(val.translation));
            m_sendPose(chan, timestamp);
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
        void m_sendPose(OSVR_ChannelCount chan,
                        util::time::TimeValue const &ts) {

            Base::d_sensor = chan;
            util::time::toStructTimeval(Base::timestamp, ts);
            char msgbuf[1000];
            vrpn_int32 len = Base::encode_to(msgbuf);
            d_connection->pack_message(len, Base::timestamp,
                                       Base::position_m_id, Base::d_sender_id,
                                       msgbuf, CLASS_OF_SERVICE);
        }
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnTrackerServer_h_GUID_15AD76EE_C97F_4FBC_1EC5_A73576D5BAAC
