/** @file
    @brief Header declaring osvr::server::Server

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

#ifndef INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
#define INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367

// Internal Includes
#include <osvr/Server/Export.h>
#include <osvr/Server/ServerPtr.h>
#include <osvr/Connection/ConnectionPtr.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <functional>

namespace osvr {
/// @brief Server functionality
/// @ingroup Server
namespace server {
    // Forward declaration for pimpl idiom.
    class ServerImpl;

    /// @brief A function that can be registered by the server app to run in
    /// each mainloop iteration.
    typedef std::function<void()> MainloopMethod;

    /// @brief Class handling a run-loop with a registration context and
    /// connection
    class Server : boost::noncopyable {
        /// @brief dummy structure to enforce private construction while
        /// permitting factory member functions to use make_shared.
        ///
        /// Based on http://stackoverflow.com/a/8147326/265522
        struct private_constructor {};

      public:
        /// @brief Private server constructor
        ///
        /// The last argument is a dummy argument to enforce use of the factory
        /// methods.
        ///
        /// @throws std::logic_error if a null connection is passed.
        Server(connection::ConnectionPtr const &conn,
               private_constructor const &);

        /// @brief Destructor - stops the loop and blocks until it's done.
        OSVR_SERVER_EXPORT ~Server();

        /// @brief Create a server object with a local-only connection.
        OSVR_SERVER_EXPORT static ServerPtr createLocal();

        /// @brief Create a server object with a provided connection.
        ///
        /// @param conn A non-null connection pointer. Since an
        /// osvr::connection::ConnectionPtr is a shared pointer, the server
        /// takes shared ownership of the connection passed.
        ///
        /// @throws std::logic_error if a null connection is passed.
        OSVR_SERVER_EXPORT static ServerPtr
        create(connection::ConnectionPtr const &conn);

        /// @brief Launch a thread running the server.
        ///
        /// @throws std::logic_error if called after the server has stopped.
        OSVR_SERVER_EXPORT void start();

        /// @brief Launch a thread running the server, and block until the
        /// server shuts down.
        ///
        /// @throws std::logic_error if called after the server has stopped.
        OSVR_SERVER_EXPORT void startAndAwaitShutdown();

        /// @brief Block until the server shuts down.
        ///
        /// Do not call from within the server thread itself, of course.
        OSVR_SERVER_EXPORT void awaitShutdown();

        /// @brief Signal the server to stop (if it is running), and block until
        /// it does so.
        ///
        /// Safe to call from any non-server thread, do not call from within the
        /// server thread itself, of course.
        OSVR_SERVER_EXPORT void stop();

        /// @brief Signal the server to stop (if it is running) but return
        /// immediately.
        ///
        /// Safe to call from any thread.
        OSVR_SERVER_EXPORT void signalStop();

        /// @brief Load plugin by name.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT void loadPlugin(std::string const &plugin);

        /// @brief Instantiate the named driver with parameters.
        /// @param plugin The name of a plugin.
        /// @param driver The name of a driver registered by the plugin for
        /// creation in this way.
        /// @param params A string containing parameters. Format is between you
        /// and the plugin, but JSON is recommended.
        ///
        /// Call only before starting the server or from within server thread.
        OSVR_SERVER_EXPORT void
        instantiateDriver(std::string const &plugin, std::string const &driver,
                          std::string const &params = std::string());

        /// @brief Run all hardware detect callbacks.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT void triggerHardwareDetect();

        /// @brief Register a method to run during every time through the main
        /// loop.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT void registerMainloopMethod(MainloopMethod f);

        /// @brief Register a JSON string as a routing directive.
        ///
        /// If the server is running, this will trigger a re-transmission of
        /// routing directives to all clients.
        ///
        /// @returns true if the route was new, or false if it replaced an
        /// existing route for that destination.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT bool addRoute(std::string const &routingDirective);

        /// @brief Get a JSON array of all routing directives.
        /// @param styled Pass `true` if you want the result pretty-printed.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT std::string getRoutes(bool styled = false) const;

        /// @brief Gets the source for a given named destination in the routing
        /// directives.
        ///
        /// Safe to call from any thread, even when server is running.
        OSVR_SERVER_EXPORT std::string
        getSource(std::string const &destination) const;

        /// @brief Sets the amount of time (in microseconds) that the server
        /// loop will sleep each loop.
        ///
        /// Call only before starting the server or from within server thread.
        OSVR_SERVER_EXPORT void setSleepTime(int microseconds);

        /// @brief Returns the amount of time (in microseconds) that the server
        /// loop sleeps each loop.
        ///
        /// Call only before starting the server or from within server thread.
        OSVR_SERVER_EXPORT int getSleepTime() const;

      private:
        unique_ptr<ServerImpl> m_impl;
    };
} // namespace server
} // namespace osvr

#endif // INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
