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

#ifndef INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982
#define INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982

// Internal Includes
#include <osvr/Server/Server.h>
#include <osvr/Common/RouteContainer.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/PluginHost/RegistrationContext_fwd.h>
#include <osvr/Connection/MessageTypePtr.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/SystemComponent_fwd.h>
#include <osvr/Common/CommonComponent_fwd.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Util/Flag.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <util/RunLoopManagerBoost.h>
#include <boost/thread.hpp>
#include <vrpn_Connection.h>
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace server {

    /// @brief Private implementation class for Server.
    class ServerImpl : boost::noncopyable {
      public:
        /// @brief Constructor
        ServerImpl(connection::ConnectionPtr const &conn);

        /// @brief Destructor (stops the thread first)
        ~ServerImpl();

        /// @brief Launch a thread running the server.
        void start();

        /// @brief Launch a thread running the server, and block until the
        /// server shuts down.
        void startAndAwaitShutdown();

        /// @copydoc Server::awaitShutdown()
        void awaitShutdown();

        /// @brief Signal the server to stop, and block until it does so.
        void stop();

        /// @brief Signal the server to stop (if it is running) but return
        /// immediately.
        void signalStop();

        /// @brief Load named plugin
        void loadPlugin(std::string const &pluginName);

        /// @brief Load all auto-loadable plugins.
        void loadAutoPlugins();

        /// @copydoc Server::setHardwareDetectOnConnection()
        void setHardwareDetectOnConnection();

        /// @copydoc Server::triggerHardwareDetect()
        void triggerHardwareDetect();

        /// @copydoc Server::registerMainloopMethod()
        void registerMainloopMethod(MainloopMethod f);

        /// @copydoc Server::addRoute()
        bool addRoute(std::string const &routingDirective);

        /// @copydoc Server::addAlias()
        bool addAlias(std::string const &path, std::string const &source,
                      common::AliasPriority priority);

        /// @copydoc Server::addAliases()
        bool addAliases(Json::Value const &aliases,
                        common::AliasPriority priority);

        /// @copydoc Server::addExternalDevice
        void addExternalDevice(std::string const &path,
                               std::string const &deviceName,
                               std::string const &server,
                               std::string const &descriptor);

        /// @copydoc Server::addString
        bool addString(std::string const &path, std::string const &value);

        /// @copydoc Server::getSource()
        std::string getSource(std::string const &destination) const;

        /// @copydoc Server::setSleepTime()
        void setSleepTime(int microseconds);

        /// @copydoc Server::getSleepTime()
        int getSleepTime() const;

        /// @copydoc Server::instantiateDriver()
        void instantiateDriver(std::string const &plugin,
                               std::string const &driver,
                               std::string const &params);

        /// @brief The method to just do the update stuff, not in a thread.
        void update();

      private:
        /// @brief The method called from the server thread repeatedly.
        /// @returns true if the loop should continue running
        bool m_loop();

        /// @brief The actual guts of the update
        void m_update();

        /// @brief Internal function to call a callable if the thread isn't
        /// running, or to queue up the callable if it is running.
        template <typename Callable> void m_callControlled(Callable f);

        /// @overload
        template <typename Callable> void m_callControlled(Callable f) const;

        /// @brief Destroy the context, connection, and nested device in a safe
        /// order.
        void m_orderedDestruction();

        /// @brief sends route message.
        void m_sendRoutes();

        /// @brief sends full path tree contents
        void m_sendTree();

        /// @brief handles updated route message from client
        static int VRPN_CALLBACK m_handleUpdatedRoute(void *userdata,
                                                      vrpn_HANDLERPARAM p);

        /// @brief adds a route - assumes that you've handled ensuring this is
        /// the main server thread.
        bool m_addRoute(std::string const &routingDirective);

        /// @brief adds an alias - assumes that you've handled ensuring this is
        /// the main server thread.
        bool m_addAlias(std::string const &path, std::string const &source,
                        common::AliasPriority priority);

        /// @brief adds aliases - assumes that you've handled ensuring this is
        /// the main server thread.
        bool m_addAliases(Json::Value const &aliases,
                          common::AliasPriority priority);

        /// @brief Handle new or updated device descriptors.
        void m_handleDeviceDescriptors();

        /// @brief Connection ownership.
        connection::ConnectionPtr m_conn;

        /// @brief Context ownership.
        shared_ptr<pluginhost::RegistrationContext> m_ctx;

        /// @brief Callbacks to call in each loop.
        std::vector<MainloopMethod> m_mainloopMethods;

        /// @brief System device
        common::BaseDevicePtr m_systemDevice;

        /// @brief System device component
        common::SystemComponent *m_systemComponent;

        /// @brief Common component for system device
        common::CommonComponent *m_commonComponent;

        /// @brief JSON routing directives
        common::RouteContainer m_routes;

        /// @brief Path tree
        common::PathTree m_tree;
        util::Flag m_treeDirty;

        /// @brief Mutex held by anything executing in the main thread.
        mutable boost::mutex m_mainThreadMutex;

        /// @brief Mutex controlling ability to check/change state of run loop
        /// @todo is mutable OK here?
        mutable boost::mutex m_runControl;
        /// @name Protected by m_runControl
        /// @{
        boost::thread m_thread;
        ::util::RunLoopManagerBoost m_run;
        bool m_running;
        bool m_everStarted = false;
        /// @}

        /// @brief Number of microseconds to sleep after each loop
        /// iteration.
        int m_sleepTime;
    };

    template <typename Callable>
    inline void ServerImpl::m_callControlled(Callable f) {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (m_running && boost::this_thread::get_id() != m_thread.get_id()) {
            boost::unique_lock<boost::mutex> lock(m_mainThreadMutex);
            f();
        } else {
            f();
        }
    }

    template <typename Callable>
    inline void ServerImpl::m_callControlled(Callable f) const {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (m_running && boost::this_thread::get_id() != m_thread.get_id()) {
            boost::unique_lock<boost::mutex> lock(m_mainThreadMutex);
            f();
        } else {
            f();
        }
    }

} // namespace server
} // namespace osvr

#endif // INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982
