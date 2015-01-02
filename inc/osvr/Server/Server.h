/** @file
    @brief Header declaring osvr::server::Server

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
        OSVR_SERVER_EXPORT void start();

        /// @brief Launch a thread running the server, and block until the
        /// server shuts down.
        OSVR_SERVER_EXPORT void startAndAwaitShutdown();

        /// @brief Signal the server to stop (if it is running), and block until
        /// it does so.
        OSVR_SERVER_EXPORT void stop();

        /// @brief Signal the server to stop (if it is running) but return
        /// immediately.
        OSVR_SERVER_EXPORT void signalStop();

        /// @brief Load plugin by name.
        OSVR_SERVER_EXPORT void loadPlugin(std::string const &plugin);

        /// @brief Instantiate the named driver with parameters.
        /// @param plugin The name of a plugin.
        /// @param driver The name of a driver registered by the plugin for
        /// creation in this way.
        /// @param params A string containing parameters. Format is between you
        /// and the plugin, but JSON is recommended.
        OSVR_SERVER_EXPORT void
        instantiateDriver(std::string const &plugin, std::string const &driver,
                          std::string const &params = std::string());

        /// @brief Run all hardware detect callbacks.
        OSVR_SERVER_EXPORT void triggerHardwareDetect();

        /// @brief Register a method to run during every time through the main
        /// loop.
        OSVR_SERVER_EXPORT void registerMainloopMethod(MainloopMethod f);

        /// @brief Register a JSON string as a routing directive.
        /// @brief Returns true if the route was new, or false if it replaced an
        /// existing route for that destination.
        OSVR_SERVER_EXPORT bool addRoute(std::string const &routingDirective);

        /// @brief Get a JSON array of all routing directives.
        /// @param styled Pass `true` if you want the result pretty-printed.
        OSVR_SERVER_EXPORT std::string getRoutes(bool styled = false) const;

      private:
        unique_ptr<ServerImpl> m_impl;
    };
} // namespace server
} // namespace osvr

#endif // INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
