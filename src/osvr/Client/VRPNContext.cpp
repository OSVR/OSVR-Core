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
#include "display_json.h"
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/QuatlibInteropC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Util/EigenInterop.h>

// Library/third-party includes
#include <vrpn_Tracker.h>

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}
    namespace {
        class TrackerSensorPredicate {
          public:
            TrackerSensorPredicate(vrpn_int32 sensor) : m_sensor(sensor) {}

            bool operator()(vrpn_TRACKERCB const &info) {
                return info.sensor == m_sensor;
            }

          private:
            vrpn_int32 m_sensor;
        };
        class AlwaysTruePredicate {
          public:
            template <typename T> bool operator()(T const &) { return true; }
        };
        class NullTrackerTransform {
          public:
            void operator()(OSVR_PoseReport &) {}
        };

        class HydraTrackerTransform {
          public:
            void operator()(OSVR_PoseReport &report) {
                Eigen::Isometry3d pose = util::fromPose(report.pose);
                // Rotate -90 about X
                util::toPose(
                    Eigen::AngleAxisd(-0.5 * M_PI, Eigen::Vector3d::UnitX()) *
                        pose,
                    report.pose);
            }
        };
    } // namespace

    template <typename Predicate, typename Transform>
    class VRPNTrackerRouter : public RouterEntry {
      public:
        VRPNTrackerRouter(ClientContext *ctx, vrpn_Connection *conn,
                          const char *src, const char *dest, Predicate p,
                          Transform t)
            : RouterEntry(ctx, dest),
              m_remote(new vrpn_Tracker_Remote(src, conn)), m_pred(p),
              m_transform(t) {
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
                self->m_transform(report);

                for (auto const &iface : self->getContext()->getInterfaces()) {
                    if (iface->getPath() == self->getDest()) {
                        iface->triggerCallbacks(timestamp, report);
                    }
                }

                /// @todo current heuristic for "do we have position data?" is
                /// "is our position non-zero?"
                if (util::vecMap(report.pose.translation) !=
                    Eigen::Vector3d::Zero()) {
                    OSVR_PositionReport positionReport;
                    positionReport.sensor = info.sensor;
                    positionReport.xyz = report.pose.translation;
                    for (auto const &iface :
                         self->getContext()->getInterfaces()) {
                        if (iface->getPath() == self->getDest()) {
                            iface->triggerCallbacks(timestamp, positionReport);
                        }
                    }
                }

                /// @todo check to see if rotation is useful/provided
                {
                    OSVR_OrientationReport oriReport;
                    oriReport.sensor = info.sensor;
                    oriReport.rotation = report.pose.rotation;
                    for (auto const &iface :
                         self->getContext()->getInterfaces()) {
                        if (iface->getPath() == self->getDest()) {
                            iface->triggerCallbacks(timestamp, oriReport);
                        }
                    }
                }
            }
        }
        void operator()() { m_remote->mainloop(); }

      private:
        unique_ptr<vrpn_Tracker_Remote> m_remote;
        Predicate m_pred;
        Transform m_transform;
    };

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice = "OSVR@" + m_host;
        m_conn = vrpn_get_connection_by_name(contextDevice.c_str());

        /// @todo this is hardcoded routing, and not well done - just a stop-gap
        /// measure.
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands/left", TrackerSensorPredicate(0),
                           HydraTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands/right", TrackerSensorPredicate(1),
                           HydraTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",
                           "/me/hands", AlwaysTruePredicate(),
                           HydraTrackerTransform());

        m_addTrackerRouter(
            "org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
            "/me/head", TrackerSensorPredicate(1));

        setParameter("/display",
                     std::string(reinterpret_cast<char *>(display_json),
                                 display_json_len));
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
        m_addTrackerRouter(src, dest, pred, NullTrackerTransform());
    }

    template <typename Predicate, typename Transform>
    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         Predicate pred, Transform xform) {
        OSVR_DEV_VERBOSE("Adding tracker route for " << dest);
        m_routers.emplace_back(new VRPNTrackerRouter<Predicate, Transform>(
            this, m_conn.get(), src, dest, pred, xform));
    }

} // namespace client
} // namespace osvr