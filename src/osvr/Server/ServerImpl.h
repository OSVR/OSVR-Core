/** @file
    @brief Header

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

#ifndef INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982
#define INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982

// Internal Includes
#include <osvr/Server/Server.h>
#include <osvr/Server/RouteContainer.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/PluginHost/RegistrationContext_fwd.h>
#include <osvr/Connection/MessageTypePtr.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/SystemComponent_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <util/RunLoopManagerBoost.h>
#include <boost/thread.hpp>

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

        /// @copydoc Server::triggerHardwareDetect()
        void triggerHardwareDetect();

        /// @copydoc Server::registerMainloopMethod()
        void registerMainloopMethod(MainloopMethod f);

        /// @copydoc Server::addRoute()
        bool addRoute(std::string const &routingDirective);

        /// @copydoc Server::getRoutes()
        std::string getRoutes(bool styled) const;

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

        /// @brief The method called from the server thread repeatedly.
        /// @returns true if the loop should continue running
        bool loop();

      private:
        /// @brief Internal function to call a callable if the thread isn't
        /// running, or to queue up the callable if it is running.
        template <typename Callable> void m_callControlled(Callable f);

        /// @overload
        template <typename Callable> void m_callControlled(Callable f) const;

        /// @brief sends route message.
        void m_sendRoutes();

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

        /// @brief JSON routing directives
        RouteContainer m_routes;

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
        /// @}

        /// @brief Number of microseconds to sleep after each loop iteration.
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
