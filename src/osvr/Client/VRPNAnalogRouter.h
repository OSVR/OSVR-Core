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
        VRPNAnalogRouter(common::ClientContext *ctx,
                         vrpn_ConnectionPtr const &conn, const char *src,
                         const char *dest, Predicate p, Transform t,
                         int channel)
            : RouterEntry(ctx, dest), m_channel(channel),
              m_remote(new vrpn_Analog_Remote(src, conn.get())), m_pred(p),
              m_transform(t), m_conn(conn) {
            m_remote->register_change_handler(this, &VRPNAnalogRouter::handle);
            m_remote->shutup = true;
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
        vrpn_ConnectionPtr m_conn;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_VRPNAnalogRouter_h_GUID_8247EACD_6ABF_4A87_59B8_AFD0722078A6
