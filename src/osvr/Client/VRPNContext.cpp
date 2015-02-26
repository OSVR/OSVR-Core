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

#include <osvr/Client/display_json.h>

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
        self->m_replaceRoutes(std::string(p.buffer, p.payload_len));
        return 0;
    }
    static const char SOURCE_KEY[] = "source";
    static const char DESTINATION_KEY[] = "destination";
    static const char SENSOR_KEY[] = "sensor";
    static const char TRACKER_KEY[] = "tracker";
    void VRPNContext::m_replaceRoutes(std::string const &routes) {

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(routes, root)) {
            throw std::runtime_error("JSON parse error: " +
                                     reader.getFormattedErrorMessages());
        }
        OSVR_DEV_VERBOSE("Replacing routes: had "
                         << m_routers.size() << ", received " << root.size());
        m_routers.clear();
        for (Json::ArrayIndex i = 0, e = root.size(); i < e; ++i) {
            Json::Value route = root[i];

            std::string dest = route[DESTINATION_KEY].asString();
            boost::optional<int> sensor;

            transform::JSONTransformVisitor xformParse(route[SOURCE_KEY]);
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