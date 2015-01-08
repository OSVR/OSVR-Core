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
#include <osvr/Transform/JSONTransformVisitor.h>
#include <osvr/Transform/ChangeOfBasis.h>
#include <osvr/Util/MessageKeys.h>
#include <osvr/Routing/RouteContainer.h>
#include <osvr/Routing/RoutingKeys.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}

    static inline transform::Transform getTransform(const char *data,
                                                    size_t len) {
        std::string json(data, len);
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(json, root)) {
            throw std::runtime_error("JSON parse error: " +
                                     reader.getFormattedErrorMessages());
        }
        transform::JSONTransformVisitor xform(root);
        return xform.getTransform();
    }

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string contextDevice =
            std::string(util::messagekeys::systemSender()) + "@" + m_host;
        /// Get connection, forcing a re-open for improved thread-safety.
        m_conn =
            vrpn_get_connection_by_name(contextDevice.c_str(), nullptr, nullptr,
                                        nullptr, nullptr, nullptr, true);

        setParameter("/display",
                     std::string(reinterpret_cast<char *>(display_json),
                                 display_json_len));

        m_conn->register_handler(
            m_conn->register_message_type(util::messagekeys::routingData()),
            &VRPNContext::m_handleRoutingMessage, static_cast<void *>(this));
    }

    VRPNContext::~VRPNContext() {}

    int VRPNContext::m_handleRoutingMessage(void *userdata,
                                            vrpn_HANDLERPARAM p) {
        VRPNContext *self = static_cast<VRPNContext *>(userdata);
        routing::RouteContainer newDirectives(
            std::string(p.buffer, p.payload_len));
        OSVR_DEV_VERBOSE("Replacing routes: had "
                         << self->m_routingDirectives.size() << ", received "
                         << newDirectives.size());
        self->m_routingDirectives = newDirectives;
        self->m_replaceRoutes();
        return 0;
    }
    static const char SENSOR_KEY[] = "sensor";
    static const char TRACKER_KEY[] = "tracker";
    void VRPNContext::m_replaceRoutes() {
        m_routers.clear();
        Json::Reader reader;
        for (auto const &routeString : m_routingDirectives.getRouteList()) {
            Json::Value route;
            if (!reader.parse(routeString, route)) {
                OSVR_DEV_VERBOSE("Got a bad route!");
                continue;
            }
            std::string dest = route[routing::keys::destination()].asString();
            boost::optional<int> sensor;

            transform::JSONTransformVisitor xformParse(
                route[routing::keys::source()]);
            Json::Value srcLeaf = xformParse.getLeaf();
            std::string srcDevice = srcLeaf[TRACKER_KEY].asString();
            // OSVR_DEV_VERBOSE("Source device: " << srcDevice);
            srcDevice.erase(begin(srcDevice)); // remove leading slash
            if (srcLeaf.isMember(SENSOR_KEY)) {
                sensor = srcLeaf[SENSOR_KEY].asInt();
            }

            m_addTrackerRouter(srcDevice.c_str(), dest.c_str(), sensor,
                               xformParse.getTransform());
        }

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

        OSVR_DEV_VERBOSE("Now have " << m_routers.size() << " routes.");
    }

    void VRPNContext::m_update() {
        // mainloop the VRPN connection.
        m_conn->mainloop();
        // Process each of the routers.
        for (auto const &p : m_routers) {
            (*p)();
        }
    }

    void VRPNContext::m_sendRoute(std::string const &route) {
        vrpn_int32 sender =
            m_conn->register_sender(util::messagekeys::appSender());
        vrpn_int32 msgType =
            m_conn->register_message_type(util::messagekeys::routeUpdate());
        struct timeval timestamp;
        vrpn_gettimeofday(&timestamp, nullptr);
        m_conn->pack_message(route.size(), timestamp, msgType, sender,
                             route.c_str(), vrpn_CONNECTION_RELIABLE);
        OSVR_DEV_VERBOSE(
            "Context does not support sending routes back to server.");
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

    void VRPNContext::m_addTrackerRouter(const char *src, const char *dest,
                                         boost::optional<int> sensor,
                                         transform::Transform const &xform) {
        OSVR_DEV_VERBOSE("Adding tracker route for " << dest);
        std::string source(src);
        if (std::string::npos != source.find('@')) {
            // We found an @ - so this is a device we need a new connection for.
            m_routers.emplace_back(
                new VRPNTrackerRouter(this, nullptr, src, sensor, dest, xform));
        } else {
            // No @: assume to be at the same location as the context.
            m_routers.emplace_back(new VRPNTrackerRouter(
                this, m_conn.get(), src, sensor, dest, xform));
        }
    }

} // namespace client
} // namespace osvr