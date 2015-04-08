/** @file
    @brief Implementation

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

// Internal Includes
#include "ServerImpl.h"
#include <osvr/Connection/Connection.h>
#include <osvr/Connection/ConnectionDevice.h>
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Util/MessageKeys.h>
#include <osvr/Connection/MessageType.h>
#include <osvr/Util/Verbosity.h>
#include "../Connection/VrpnConnectionKind.h" /// @todo warning - cross-library internal header!
#include <osvr/Util/Microsleep.h>
#include <osvr/Common/SystemComponent.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
#include <stdexcept>
#include <functional>

namespace osvr {
namespace server {
    static vrpn_ConnectionPtr
    getVRPNConnection(connection::ConnectionPtr const &conn) {
        vrpn_ConnectionPtr ret;
        if (std::string(conn->getConnectionKindID()) ==
            osvr::connection::getVRPNConnectionKindID()) {
            ret = vrpn_ConnectionPtr(
                static_cast<vrpn_Connection *>(conn->getUnderlyingObject()));
        }
        return ret;
    }
    ServerImpl::ServerImpl(connection::ConnectionPtr const &conn)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()),
          m_systemComponent(nullptr), m_running(false), m_sleepTime(0) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        osvr::connection::Connection::storeConnection(*m_ctx, m_conn);

        // Set up system device/system component
        auto vrpnConn = getVRPNConnection(m_conn);
        m_systemDevice = common::createServerDevice(
            common::SystemComponent::deviceName(), vrpnConn);

        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
        m_systemComponent->registerClientRouteUpdateHandler(
            &ServerImpl::m_handleUpdatedRoute, this);

        // Things to do when we get a new incoming connection
        m_conn->registerConnectionHandler(
            std::bind(&ServerImpl::triggerHardwareDetect, std::ref(*this)));
        m_conn->registerConnectionHandler(
            std::bind(&ServerImpl::m_sendRoutes, std::ref(*this)));

        // Deal with updated device descriptors.
        m_conn->registerDescriptorHandler(
            std::bind(&ServerImpl::m_handleDeviceDescriptors, std::ref(*this)));
    }

    ServerImpl::~ServerImpl() { stop(); }

    void ServerImpl::start() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (!m_ctx || !m_conn) {
            throw std::logic_error("Cannot start server - context or "
                                   "connection destroyed (probably attempting "
                                   "to restart a stopped server)");
        }
        m_running = true;

        // Use a lambda to run the loop.
        m_thread = boost::thread([&] {
            bool keepRunning = true;
            ::util::LoopGuard guard(m_run);
            do {
                keepRunning = this->loop();
            } while (keepRunning);
            m_orderedDestruction();
            m_running = false;
        });
        m_run.signalAndWaitForStart();
    }

    void ServerImpl::startAndAwaitShutdown() {
        start();
        awaitShutdown();
    }

    void ServerImpl::awaitShutdown() { m_thread.join(); }

    void ServerImpl::stop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalAndWaitForShutdown();
        m_thread.join();
        m_thread = boost::thread();
    }

    void ServerImpl::signalStop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalShutdown();
    }

    void ServerImpl::loadPlugin(std::string const &pluginName) {
        m_callControlled(std::bind(&pluginhost::RegistrationContext::loadPlugin,
                                   m_ctx, pluginName));
    }

    void ServerImpl::loadAutoPlugins() { m_ctx->loadPlugins(); }

    void ServerImpl::instantiateDriver(std::string const &plugin,
                                       std::string const &driver,
                                       std::string const &params) {
        m_ctx->instantiateDriver(plugin, driver, params);
    }

    void ServerImpl::triggerHardwareDetect() {
        OSVR_DEV_VERBOSE("Performing hardware auto-detection.");
        m_callControlled(std::bind(
            &pluginhost::RegistrationContext::triggerHardwareDetect, m_ctx));
    }

    void ServerImpl::registerMainloopMethod(MainloopMethod f) {
        if (f) {
            m_callControlled([&] { m_mainloopMethods.push_back(f); });
        }
    }

    bool ServerImpl::loop() {
        bool shouldContinue;
        {
            /// @todo More elegant way of running queued things than grabbing a
            /// mutex each time through?
            boost::unique_lock<boost::mutex> lock(m_mainThreadMutex);
            m_conn->process();
            m_systemDevice->update();
            for (auto &f : m_mainloopMethods) {
                f();
            }
            shouldContinue = m_run.shouldContinue();
        }

        if (m_sleepTime > 0) {
            osvr::util::time::microsleep(m_sleepTime);
        } else {
            m_thread.yield();
        }
        return shouldContinue;
    }

    bool ServerImpl::addRoute(std::string const &routingDirective) {
        bool wasNew;
        m_callControlled([&] { wasNew = m_addRoute(routingDirective); });
        return wasNew;
    }

    std::string ServerImpl::getRoutes(bool styled) const {
        std::string ret;
        m_callControlled([&] { ret = m_routes.getRoutes(styled); });
        return ret;
    }

    std::string ServerImpl::getSource(std::string const &destination) const {
        std::string ret;
        m_callControlled([&] { ret = m_routes.getSource(destination); });
        return ret;
    }

    void ServerImpl::m_orderedDestruction() {
        m_ctx.reset();
        m_systemComponent = nullptr; // non-owning pointer
        m_systemDevice.reset();
        m_conn.reset();
    }

    void ServerImpl::m_sendRoutes() {
        std::string message = m_routes.getRoutes();
        OSVR_DEV_VERBOSE("Transmitting " << m_routes.size()
                                         << " routes to the client.");
        m_systemComponent->sendRoutes(message);
    }

    int ServerImpl::m_handleUpdatedRoute(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<ServerImpl *>(userdata);
        OSVR_DEV_VERBOSE("Got an updated route from a client.");
        self->m_addRoute(std::string(p.buffer, p.payload_len));
        return 0;
    }

    bool ServerImpl::m_addRoute(std::string const &routingDirective) {
        bool wasNew = m_routes.addRoute(routingDirective);
        if (m_running) {
            m_sendRoutes();
        }
        return wasNew;
    }

    void ServerImpl::setSleepTime(int microseconds) {
        m_sleepTime = microseconds;
    }

    int ServerImpl::getSleepTime() const { return m_sleepTime; }

    void ServerImpl::m_handleDeviceDescriptors() {
        for (auto const &dev : m_conn->getDevices()) {
            auto const &descriptor = dev->getDeviceDescriptor();
            if (descriptor.empty()) {
                OSVR_DEV_VERBOSE("Developer Warning: No device descriptor for "
                                 << dev->getName());
            } else {
                OSVR_DEV_VERBOSE("Descriptor for " << dev->getName() << "\n"
                                                   << descriptor);
            }
        }
    }

} // namespace server
} // namespace osvr
