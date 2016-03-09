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
#include <osvr/Common/CommonComponent.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/ProcessDeviceDescriptor.h>
#include <osvr/Common/AliasProcessor.h>
#include <osvr/Util/StringLiteralFileToString.h>
#include <osvr/Common/Tracing.h>
#include <osvr/Util/PortFlags.h>

#include "osvr/Server/display_json.h" /// Fallback display descriptor.

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <boost/variant.hpp>
#include <json/reader.h>

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
    ServerImpl::ServerImpl(connection::ConnectionPtr const &conn,
                           boost::optional<std::string> const &host,
                           boost::optional<int> const &port)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()),
          m_host(host.get_value_or("localhost")),
          m_port(port.get_value_or(util::UseDefaultPort)) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        osvr::connection::Connection::storeConnection(*m_ctx, m_conn);

        // Get the underlying VRPN connection, and make sure it's OK.
        auto vrpnConn = getVRPNConnection(m_conn);

        if (!(vrpnConn->doing_okay())) {
            throw ServerCreationFailure();
        }

        // Set up system device/system component
        m_systemDevice = common::createServerDevice(
            common::SystemComponent::deviceName(), vrpnConn);

        m_systemComponent =
            m_systemDevice->addComponent(common::SystemComponent::create());
        m_systemComponent->registerClientRouteUpdateHandler(
            &ServerImpl::m_handleUpdatedRoute, this);

        // Things to do when we get a new incoming connection
        // No longer doing hardware detect unconditionally here - see
        // triggerHardwareDetect()
        m_commonComponent =
            m_systemDevice->addComponent(common::CommonComponent::create());
        m_commonComponent->registerPingHandler([&] { m_queueTreeSend(); });

        // Set up the default display descriptor.
        m_tree.getNodeByPath("/display").value() =
            common::elements::StringElement(util::makeString(display_json));
        // Deal with updated device descriptors.
        m_conn->registerDescriptorHandler([&] { m_handleDeviceDescriptors(); });

        // Set up handlers to enter/exit idle sleep mode.
        // Can't do this with the nice wrappers on the CommonComponent of the
        // system device, I suppose since people aren't really connecting to
        // that device.
        vrpnConn->register_handler(
            vrpnConn->register_message_type(vrpn_got_first_connection),
            &ServerImpl::m_exitIdle, this);
        vrpnConn->register_handler(
            vrpnConn->register_message_type(vrpn_dropped_last_connection),
            &ServerImpl::m_enterIdle, this);
    }

    ServerImpl::~ServerImpl() {
        stop();
        // Not unregistering idle handlers because doing so caused crashes, and
        // I think that this object should outlive the connection anyway.
    }

    void ServerImpl::start() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (!m_ctx || !m_conn) {
            throw std::logic_error("Cannot start server - context or "
                                   "connection destroyed (probably attempting "
                                   "to restart a stopped server)");
        }
        m_everStarted = true;
        m_running = true;

        // Use a lambda to run the loop.
        m_thread = boost::thread([&] {
            bool keepRunning = true;
            m_mainThreadId = m_thread.get_id();
            ::util::LoopGuard guard(m_run);
            do {
                keepRunning = this->m_loop();
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
        if (m_everStarted) {
            m_run.signalAndWaitForShutdown();
            m_thread.join();
            m_thread = boost::thread();
        } else {
            m_orderedDestruction();
        }
    }

    void ServerImpl::signalStop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalShutdown();
    }

    void ServerImpl::loadPlugin(std::string const &pluginName) {
        m_callControlled([&, pluginName] { m_ctx->loadPlugin(pluginName); });
    }

    void ServerImpl::loadAutoPlugins() { m_ctx->loadPlugins(); }

    void ServerImpl::setHardwareDetectOnConnection() {
        m_commonComponent->registerPingHandler(
            [&] { triggerHardwareDetect(); });
    }

    void ServerImpl::instantiateDriver(std::string const &plugin,
                                       std::string const &driver,
                                       std::string const &params) {
        BOOST_ASSERT_MSG(m_inServerThread(),
                         "This method is only available in the server thread!");
        m_ctx->instantiateDriver(plugin, driver, params);
    }

    void ServerImpl::triggerHardwareDetect() {
        m_callControlled([&] { m_triggeredDetect = true; });
    }

    void ServerImpl::registerMainloopMethod(MainloopMethod f) {
        if (f) {
            m_callControlled([&] { m_mainloopMethods.push_back(f); });
        }
    }

    void ServerImpl::update() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (m_everStarted) {
            throw std::logic_error("Can't call update() if you've ever started "
                                   "the server in its own thread!");
        }
        m_update();
    }
    void ServerImpl::m_update() {
        osvr::common::tracing::ServerUpdate trace;
        m_conn->process();
        m_systemDevice->update();
        for (auto &f : m_mainloopMethods) {
            f();
        }
        if (m_triggeredDetect) {
            OSVR_DEV_VERBOSE("Performing hardware auto-detection.");
            common::tracing::markHardwareDetect();
            m_ctx->triggerHardwareDetect();
            m_triggeredDetect = false;
        }
        if (m_treeDirty) {
            OSVR_DEV_VERBOSE("Path tree updated or connection detected");
            m_sendTree();
            m_treeDirty.reset();
        }
    }

    bool ServerImpl::m_loop() {
        bool shouldContinue;
        {
            /// @todo More elegant way of running queued things than grabbing a
            /// mutex each time through?
            boost::unique_lock<boost::mutex> lock(m_mainThreadMutex);
            m_update();
            shouldContinue = m_run.shouldContinue();
        }

        if (m_currentSleepTime > 0) {
            osvr::util::time::microsleep(m_currentSleepTime);
        }
        return shouldContinue;
    }

    bool ServerImpl::addRoute(std::string const &routingDirective) {
        bool wasNew;
        m_callControlled([&] { wasNew = m_addRoute(routingDirective); });
        return wasNew;
    }

    bool ServerImpl::addAlias(std::string const &path,
                              std::string const &source,
                              common::AliasPriority priority) {

        bool wasChanged;
        m_callControlled(
            [&] { wasChanged = m_addAlias(path, source, priority); });
        return wasChanged;
    }

    bool ServerImpl::addAliases(Json::Value const &aliases,
                                common::AliasPriority priority) {
        bool wasChanged;
        m_callControlled([&] { wasChanged = m_addAliases(aliases, priority); });
        return wasChanged;
    }

    void ServerImpl::addExternalDevice(std::string const &path,
                                       std::string const &deviceName,
                                       std::string const &server,
                                       std::string const &descriptor) {
        Json::Value descriptorVal;
        Json::Reader reader;
        if (!reader.parse(descriptor, descriptorVal)) {
            BOOST_ASSERT_MSG(0, "Should never get here - string descriptor "
                                "handed to us should have been generated from "
                                "a JSON value.");
            return;
        }
        m_callControlled([&] {
            /// Get the node
            auto &node = m_tree.getNodeByPath(path);

            /// Create the DeviceElement and set it as the node value - assume
            /// it's a VRPN device by default.
            auto elt = common::elements::DeviceElement::createVRPNDeviceElement(
                deviceName, server);
            elt.getDescriptor() = descriptorVal;
            node.value() = elt;
            m_treeDirty.set();

            /// Process device descriptor
            common::processDeviceDescriptorFromExistingDevice(node, elt);
        });
    }

    bool ServerImpl::addString(std::string const &path,
                               std::string const &value) {
        bool wasChanged = false;
        auto newElement =
            common::PathElement{common::elements::StringElement{value}};
        m_callControlled([&] {
            auto &node = m_tree.getNodeByPath(path);
            if (!(newElement == node.value())) {
                m_treeDirty.set();
                wasChanged = true;
                node.value() = newElement;
            }
        });
        return wasChanged;
    }

    void ServerImpl::m_orderedDestruction() {
        m_ctx.reset();
        m_systemComponent = nullptr; // non-owning pointer
        m_systemDevice.reset();
        m_conn.reset();
    }

    int ServerImpl::m_handleUpdatedRoute(void *userdata, vrpn_HANDLERPARAM p) {
        auto self = static_cast<ServerImpl *>(userdata);
        BOOST_ASSERT_MSG(
            self->m_inServerThread(),
            "This callback should never happen outside the server thread!");
        OSVR_DEV_VERBOSE("Got an updated route from a client.");
        self->m_addRoute(std::string(p.buffer, p.payload_len));
        return 0;
    }

    bool ServerImpl::m_addRoute(std::string const &routingDirective) {
        bool change =
            common::addAliasFromRoute(m_tree.getRoot(), routingDirective);
        m_treeDirty += change;
        return change;
    }

    bool ServerImpl::m_addAlias(std::string const &path,
                                std::string const &source,
                                common::AliasPriority priority) {
        /// @todo Handle this one with AliasProcessor.
        auto &node = m_tree.getNodeByPath(path);
        bool change = common::addAlias(node, source, priority);
        m_treeDirty += change;
        return change;
    }

    bool ServerImpl::m_addAliases(Json::Value const &aliases,
                                  common::AliasPriority priority) {
        bool change = common::AliasProcessor()
                          .setDefaultPriority(priority)
                          .enableWildcard()
                          .process(m_tree.getRoot(), aliases);
        m_treeDirty += change;
        return change;
    }
    void ServerImpl::m_queueTreeSend() {
        m_callControlled([&] { m_treeDirty += true; });
    }
    void ServerImpl::m_sendTree() {
        OSVR_DEV_VERBOSE("Sending path tree to clients.");
        common::tracing::markPathTreeBroadcast();
        m_systemComponent->sendReplacementTree(m_tree);
    }

    void ServerImpl::setSleepTime(int microseconds) {
        m_sleepTime = microseconds;
    }
#if 0
    int ServerImpl::getSleepTime() const { return m_sleepTime; }
#endif
    void ServerImpl::m_handleDeviceDescriptors() {
        for (auto const &dev : m_conn->getDevices()) {
            auto const &descriptor = dev->getDeviceDescriptor();
            if (descriptor.empty()) {
                OSVR_DEV_VERBOSE("Developer Warning: No device descriptor for "
                                 << dev->getName());
            } else {
                m_treeDirty += common::processDeviceDescriptorForPathTree(
                    m_tree, dev->getName(), descriptor, m_port, m_host);
            }
        }
    }

    int ServerImpl::m_exitIdle(void *userdata, vrpn_HANDLERPARAM) {
        auto self = static_cast<ServerImpl *>(userdata);
        /// Conditional ensures that we don't "idle" faster than we run: Make
        /// sure we're sleeping longer now than we will be once we exit idle.
        if (self->m_currentSleepTime > self->m_sleepTime) {
            OSVR_DEV_VERBOSE("Got first client connection, exiting idle mode.");
            self->m_currentSleepTime = self->m_sleepTime;
        }
        return 0;
    }

    int ServerImpl::m_enterIdle(void *userdata, vrpn_HANDLERPARAM) {
        auto self = static_cast<ServerImpl *>(userdata);

        /// Conditional ensures that we don't "idle" faster than we run: Make
        /// sure we're sleeping shorter now than we will be once we enter idle.
        if (self->m_currentSleepTime < IDLE_SLEEP_TIME) {
            OSVR_DEV_VERBOSE(
                "Dropped last client connection, entering idle mode.");
            self->m_currentSleepTime = IDLE_SLEEP_TIME;
        }
        return 0;
    }

} // namespace server
} // namespace osvr
