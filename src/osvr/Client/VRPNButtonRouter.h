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
        VRPNButtonRouter(ClientContext *ctx, vrpn_ConnectionPtr conn,
                         const char *src, const char *dest, Predicate p)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Button_Remote(src, conn.get())), m_pred(p),
              m_conn(conn) {
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
        vrpn_ConnectionPtr m_conn;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNButtonRouter_h_GUID_C504B3E6_E62D_4B85_E3B7_3A25A2F678B3
