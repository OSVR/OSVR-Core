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

#ifndef INCLUDED_VRPNButtonRouter_h_GUID_C504B3E6_E62D_4B85_E3B7_3A25A2F678B3
#define INCLUDED_VRPNButtonRouter_h_GUID_C504B3E6_E62D_4B85_E3B7_3A25A2F678B3

// Internal Includes
// - none

// Library/third-party includes
#include <vrpn_Button.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    template <typename Predicate> class VRPNButtonRouter : public RouterEntry {
      public:
        VRPNButtonRouter(ClientContext *ctx, vrpn_Connection *conn,
                         const char *src, const char *dest, Predicate p)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Button_Remote(src, conn)), m_pred(p) {
            m_remote->register_change_handler(this, &VRPNButtonRouter::handle);
            m_remote->shutup = true;
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_BUTTONCB info) {
            VRPNButtonRouter *self = static_cast<VRPNButtonRouter *>(userdata);
            if (self->m_pred(info)) {
                OSVR_ButtonReport report;
                report.sensor = info.button;
                report.state = static_cast<uint8_t>(info.state);
                OSVR_TimeValue timestamp;
                osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, report);
                    }
                }
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Button_Remote> m_remote;
        Predicate m_pred;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNButtonRouter_h_GUID_C504B3E6_E62D_4B85_E3B7_3A25A2F678B3
