/** @file
    @brief Implementation

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

// Internal Includes
#include "VRPNContext.h"
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/QuatlibInteropC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <vrpn_Tracker.h>

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}

    template <typename Predicate> class VRPNTrackerRouter : public RouterEntry {
      public:
        VRPNTrackerRouter(ClientContext *ctx, vrpn_Connection *conn,
                          const char *src, const char *dest, Predicate p)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Tracker_Remote(src, conn)), m_pred(p) {
            m_remote->register_change_handler(this, &VRPNTrackerRouter::handle);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {
            VRPNTrackerRouter *self =
                static_cast<VRPNTrackerRouter *>(userdata);
            if (self->m_pred(info)) {
                OSVR_PoseReport report;
                report.sensor = info.sensor;
                OSVR_TimeValue timestamp;
                osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
                osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
                osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, report);
                    }
                }
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        Predicate m_pred;
    };

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice = "OSVR@" + m_host;
        m_conn = vrpn_get_connection_by_name(contextDevice.c_str());

        /// @todo this is hardcoded routing, and not well done - just a stop-gap
        /// measure.
        m_addTrackerRouter(
            "org_opengoggles_bundled_Multiserver/RazerHydra0", "/me/hands/left",
            [](vrpn_TRACKERCB const &info) { return info.sensor == 0; });
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands/right", [](vrpn_TRACKERCB const &info) {
            return info.sensor == 1;
        });
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands",
                           [](vrpn_TRACKERCB const &) { return true; });

        m_addTrackerRouter(
            "org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
            "/me/head",
            [](vrpn_TRACKERCB const &info) { return info.sensor == 0; });
    }

    VRPNContext::~VRPNContext() {}

    void VRPNContext::m_update() {
        // mainloop the VRPN connection.
        m_conn->mainloop();
        // Process each of the routers.
        for (auto const &p : m_routers) {
            (*p)();
        }
    }

    template <typename Predicate>
    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         Predicate pred) {
        OSVR_DEV_VERBOSE("Adding tracker route for " << dest);
        m_routers.emplace_back(new VRPNTrackerRouter<Predicate>(
            this, m_conn.get(), src, dest, pred));
    }

} // namespace client
} // namespace osvr