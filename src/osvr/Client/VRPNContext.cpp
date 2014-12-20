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
#include "RouterPredicates.h"
#include "RouterTransforms.h"
#include "VRPNAnalogRouter.h"
#include "VRPNButtonRouter.h"
#include "VRPNTrackerRouter.h"
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice = "OSVR@" + m_host;
        m_conn = vrpn_get_connection_by_name(contextDevice.c_str());

        /// @todo this is hardcoded routing, and not well done - just a stop-gap
        /// measure. This one-euro filter connects to the hydra.
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/OneEuroFilter0",
                           "/me/hands/left", SensorPredicate(0),
                           ZUpTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/OneEuroFilter0",
                           "/me/hands/right", SensorPredicate(1),
                           ZUpTrackerTransform());
        m_addTrackerRouter("org_opengoggles_bundled_Multiserver/OneEuroFilter0",
                           "/me/hands", AlwaysTruePredicate(),
                           ZUpTrackerTransform());

        m_addTrackerRouter(
            "org_opengoggles_bundled_Multiserver/YEI_3Space_Sensor0",
            "/me/head", SensorPredicate(1),
            combineTransforms(
                CustomPostrotateTransform(-.5 * M_PI, Eigen::Vector3d::UnitZ()),
                combineTransforms(ZUpTrackerTransform(),
                                  CustomPostrotateTransform(
                                      .5 * M_PI, Eigen::Vector3d::UnitX()))));

#define OSVR_HYDRA_BUTTON(SENSOR, NAME)                                        \
    m_addButtonRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",       \
                      "/controller/left/" NAME, SensorPredicate(SENSOR));      \
    m_addButtonRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",       \
                      "/controller/right/" NAME, SensorPredicate(SENSOR + 8))

        OSVR_HYDRA_BUTTON(0, "middle");
        OSVR_HYDRA_BUTTON(1, "1");
        OSVR_HYDRA_BUTTON(2, "2");
        OSVR_HYDRA_BUTTON(3, "3");
        OSVR_HYDRA_BUTTON(4, "4");
        OSVR_HYDRA_BUTTON(5, "bumper");
        OSVR_HYDRA_BUTTON(6, "joystick/button");

#undef OSVR_HYDRA_BUTTON

#define OSVR_HYDRA_ANALOG(SENSOR, NAME)                                        \
    m_addAnalogRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",       \
                      "/controller/left/" NAME, SENSOR);                       \
    m_addAnalogRouter("org_opengoggles_bundled_Multiserver/RazerHydra0",       \
                      "/controller/right/" NAME, SENSOR + 3)

        OSVR_HYDRA_ANALOG(0, "joystick/x");
        OSVR_HYDRA_ANALOG(1, "joystick/y");
        OSVR_HYDRA_ANALOG(2, "trigger");

#undef OSVR_HYDRA_ANALOG

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

    void VRPNContext::m_addAnalogRouter(const char *src, const char *dest,
                                        int channel) {
        OSVR_DEV_VERBOSE("Adding analog route for " << dest);

        m_routers.emplace_back(
            new VRPNAnalogRouter<SensorPredicate, NullTransform>(
                this, m_conn.get(), src, dest, SensorPredicate(channel),
                NullTransform(), channel));
    }

    template <typename Predicate>
    void VRPNContext::m_addButtonRouter(const char *src, const char *dest,
                                        Predicate pred) {
        OSVR_DEV_VERBOSE("Adding button route for " << dest);
        m_routers.emplace_back(new VRPNButtonRouter<Predicate>(
            this, m_conn.get(), src, dest, pred));
    }

    template <typename Predicate>
    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         Predicate pred) {
        m_addTrackerRouter(src, dest, pred, NullTransform());
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