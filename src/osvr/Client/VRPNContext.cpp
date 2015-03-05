/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "VRPNContext.h"
#include "RouterPredicates.h"
#include "RouterTransforms.h"
#include "ImagingRouter.h"
#include "VRPNAnalogRouter.h"
#include "VRPNButtonRouter.h"
#include "VRPNTrackerRouter.h"
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/Client/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Transform/JSONTransformVisitor.h>
#include <osvr/Transform/ChangeOfBasis.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/SystemComponent.h>
#include <osvr/Common/RoutingKeys.h>

#include <osvr/Client/display_json.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// Standard includes
#include <cstring>

namespace osvr {
namespace client {
    RouterEntry::~RouterEntry() {}

    VRPNContext::VRPNContext(const char appId[], const char host[])
        : ::OSVR_ClientContextObject(appId), m_host(host) {

        std::string sysDeviceName =
            std::string(common::SystemComponent::deviceName()) + "@" + m_host;
        /// Get connection, forcing a re-open for improved thread-safety.
        m_conn =
            vrpn_get_connection_by_name(sysDeviceName.c_str(), nullptr, nullptr,
                                        nullptr, nullptr, nullptr, true);
        m_conn->removeReference(); // Remove extra reference.

        /// Create the system client device.
        m_systemDevice = common::createClientDevice(sysDeviceName, m_conn);
        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
        m_systemComponent->registerRoutesHandler(
            &VRPNContext::m_handleRoutingMessage, static_cast<void *>(this));

        setParameter("/display",
                     std::string(display_json, sizeof(display_json)));
    }

    VRPNContext::~VRPNContext() {}

    int VRPNContext::m_handleRoutingMessage(void *userdata,
                                            vrpn_HANDLERPARAM p) {
        VRPNContext *self = static_cast<VRPNContext *>(userdata);
        common::RouteContainer newDirectives(
            std::string(p.buffer, p.payload_len));
        self->m_replaceRoutes(newDirectives);
        return 0;
    }

    void
    VRPNContext::m_replaceRoutes(common::RouteContainer const &newDirectives) {
        OSVR_DEV_VERBOSE("Replacing routing directives: had "
                         << m_routingDirectives.size() << ", received "
                         << newDirectives.size());

        m_routers.clear();
        m_routingDirectives = newDirectives;
        Json::Reader reader;
        for (auto const &routeString : newDirectives.getRouteList()) {
            Json::Value route;
            if (!reader.parse(routeString, route)) {
                OSVR_DEV_VERBOSE("Got a bad route!");
                continue;
            }

            std::string dest =
                route[common::routing_keys::destination()].asString();

            Json::Value src = route[common::routing_keys::source()];
            if (src.isString()) {
                m_handleStringRouteEntry(dest, src.asString());
            } else {
                m_handleTrackerRouteEntry(dest, src);
            }
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

    static const char SENSOR_KEY[] = "sensor";
    static const char TRACKER_KEY[] = "tracker";
    void VRPNContext::m_handleTrackerRouteEntry(std::string const &dest,
                                                Json::Value src) {

        boost::optional<int> sensor;
        transform::JSONTransformVisitor xformParse(src);
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

    static const char IMAGING_NAME[] = "imaging";
    void VRPNContext::m_handleStringRouteEntry(std::string const &dest,
                                               std::string src) {
        std::vector<std::string> components;

        /// @todo replace literal with getPathSeparator
        boost::algorithm::split(components, src,
                                boost::algorithm::is_any_of("/"));
        if (!components.empty()) {
            if (components.front().empty()) {
                components.erase(begin(components));
            }
        }
        if (components.size() < 4) {
            OSVR_DEV_VERBOSE("Could not parse source for route, skipping: "
                             << src << " => " << dest);
            return;
        }

        std::string deviceName =
            components[0] + "/" + components[1] + "@" + m_host;
        std::reverse(begin(components), end(components));
        components.pop_back();
        components.pop_back();

        std::string interfaceType = components.back();
        components.pop_back();
        if (interfaceType == IMAGING_NAME) {
            OSVR_DEV_VERBOSE("Adding imaging route for " << dest);
            m_routers.emplace_back(
                new ImagingRouter(this, m_conn, deviceName, components, dest));
        } else {
            OSVR_DEV_VERBOSE(
                "Could not handle route message for interface type "
                << interfaceType << ", skipping: " << src << " => " << dest);
        }
    }

    void VRPNContext::m_sendRoute(std::string const &route) {
        m_systemComponent->sendClientRouteUpdate(route);
    }

    void VRPNContext::m_update() {
        // mainloop the VRPN connection.
        m_conn->mainloop();
        // Mainloop the system device
        m_systemDevice->update();

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
                this, m_conn.get(), (src + ("@" + m_host)).c_str(), dest,
                SensorPredicate(channel), NullTransform(), channel));
    }

    template <typename Predicate>
    void VRPNContext::m_addButtonRouter(const char *src, const char *dest,
                                        Predicate pred) {
        OSVR_DEV_VERBOSE("Adding button route for " << dest);
        m_routers.emplace_back(new VRPNButtonRouter<Predicate>(
            this, m_conn.get(), (src + ("@" + m_host)).c_str(), dest, pred));
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
                this, m_conn.get(), (src + ("@" + m_host)).c_str(), sensor,
                dest, xform));
        }
    }

} // namespace client
} // namespace osvr