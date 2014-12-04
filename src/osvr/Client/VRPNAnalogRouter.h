/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VRPNAnalogRouter_h_GUID_8247EACD_6ABF_4A87_59B8_AFD0722078A6
#define INCLUDED_VRPNAnalogRouter_h_GUID_8247EACD_6ABF_4A87_59B8_AFD0722078A6

// Internal Includes
// - none

// Library/third-party includes
#include <vrpn_Analog.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    template <typename Predicate, typename Transform>
    class VRPNAnalogRouter : public RouterEntry {
      public:
        VRPNAnalogRouter(ClientContext *ctx, vrpn_Connection *conn,
                         const char *src, const char *dest, Predicate p,
                         Transform t)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Analog_Remote(src, conn)), m_pred(p),
              m_transform(t) {
            m_remote->register_change_handler(this, &VRPNAnalogRouter::handle);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_ANALOGCB info) {
            VRPNAnalogRouter *self = static_cast<VRPNAnalogRouter *>(userdata);
#if 0
                if (self->m_pred(info)) {
                    OSVR_PoseReport report;
                    report.sensor = info.sensor;
                    OSVR_TimeValue timestamp;
                    osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
                    osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
                    osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
                    self->m_transform(report);

                    for (auto const &iface : self->getContext()->getInterfaces()) {
                        if (iface->getPath() == self->getDest()) {
                            iface->triggerCallbacks(timestamp, report);
                        }
                    }
                }
#endif
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Analog_Remote> m_remote;
        Predicate m_pred;
        Transform m_transform;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNAnalogRouter_h_GUID_8247EACD_6ABF_4A87_59B8_AFD0722078A6
