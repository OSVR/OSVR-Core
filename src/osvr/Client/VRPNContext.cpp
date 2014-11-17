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
#include <boost/range/algorithm.hpp>

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    CallableObject::~CallableObject() {}
    template <typename Predicate> class VRPNRouter : public CallableObject {
      public:
        VRPNRouter(const char *src, vrpn_Connection *conn, const char *dest,
                   ClientContext *ctx, Predicate p)
            : m_remote(new vrpn_Tracker_Remote(src, conn)), m_dest(dest),
              m_pred(p), m_ctx(ctx) {
            m_remote->register_change_handler(this, &VRPNRouter::handle);
        }

        static void VRPN_CALLBACK handle(void *userdata, vrpn_TRACKERCB info) {

            VRPNRouter *self = static_cast<VRPNRouter *>(userdata);
            if (self->m_pred(info)) {
                OSVR_PoseReport report;
                report.sensor = info.sensor;
                OSVR_TimeValue timestamp;
                osvrStructTimevalToTimeValue(&timestamp, &(info.msg_time));
                osvrQuatFromQuatlib(&(report.pose.rotation), info.quat);
                osvrVec3FromQuatlib(&(report.pose.translation), info.pos);
                boost::for_each(self->m_ctx->getInterfaces(),
                                [&](ClientInterfacePtr const &iface) {
                    iface->triggerCallbacks(timestamp, report);
                });
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        std::string const m_dest;
        Predicate m_pred;
        ClientContext *m_ctx;
    };

    template <typename Predicate>
    inline unique_ptr<CallableObject>
    createRouter(const char *src, vrpn_Connection *conn, const char *dest,
                 ClientContext *ctx, Predicate p) {
        unique_ptr<CallableObject> ret(
            new VRPNRouter<Predicate>(src, conn, dest, ctx, p));
        return ret;
    }

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice = "OSVR@" + m_host;
        m_conn = vrpn_get_connection_by_name(contextDevice.c_str());

        /// @todo this is hardcoded routing, and not well done - just a stop-gap
        /// measure.
        m_routers.push_back(createRouter(
            "org_opengoggles_bundled_Multiserver/RazerHydra0", m_conn.get(),
            "/me/hands/left", this,
            [](vrpn_TRACKERCB const &info) { return info.sensor == 0; }));
        m_routers.push_back(createRouter(
            "org_opengoggles_bundled_Multiserver/RazerHydra0", m_conn.get(),
            "/me/hands/right", this,
            [](vrpn_TRACKERCB const &info) { return info.sensor == 1; }));
        m_routers.push_back(createRouter(
            "org_opengoggles_bundled_Multiserver/RazerHydra0", m_conn.get(),
            "/me/hands", this, [](vrpn_TRACKERCB const &) { return true; }));
    }

    VRPNContext::~VRPNContext() {}

    void VRPNContext::m_update() {
        m_conn->mainloop();
        boost::for_each(m_routers, [](CallablePtr const &p) { (*p)(); });
    }

} // namespace client
} // namespace osvr