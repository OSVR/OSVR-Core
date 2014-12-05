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
                         Transform t, int channel)
            : RouterEntry(ctx, dest), m_channel(channel),
              m_remote(new vrpn_Analog_Remote(src, conn)), m_pred(p),
              m_transform(t) {
            m_remote->register_change_handler(this, &VRPNAnalogRouter::handle);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_ANALOGCB info) {
            VRPNAnalogRouter *self = static_cast<VRPNAnalogRouter *>(userdata);
            if (self->m_pred(info)) {
                OSVR_TimeValue timestamp;
                osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));

                OSVR_AnalogReport report;
                report.sensor = self->m_channel;
                report.state = info.channel[self->m_channel];
                self->m_transform(report);

                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, report);
                    }
                }
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        int m_channel;
        unique_ptr<vrpn_Analog_Remote> m_remote;
        Predicate m_pred;
        Transform m_transform;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNAnalogRouter_h_GUID_8247EACD_6ABF_4A87_59B8_AFD0722078A6
