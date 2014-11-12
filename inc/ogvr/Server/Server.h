/** @file
    @brief Header declaring ogvr::server::Server

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
#include <ogvr/Server/Export.h>
#include <ogvr/Server/ServerPtr.h>
#include <ogvr/Connection/ConnectionPtr.h>
#include <ogvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace ogvr {
/// @brief Server functionality
/// @ingroup Server
namespace server {
    // Forward declaration for pimpl idiom.
    class ServerImpl;

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
        OGVR_SERVER_EXPORT ~Server();

        /// @brief Create a server object with a local-only connection.
        OGVR_SERVER_EXPORT static ServerPtr createLocal();

        /// @brief Create a server object with a provided connection.
        ///
        /// @param conn A non-null connection pointer. Since an
        /// ogvr::connection::ConnectionPtr is a shared pointer, the server
        /// takes shared ownership of the connection passed.
        ///
        /// @throws std::logic_error if a null connection is passed.
        OGVR_SERVER_EXPORT static ServerPtr
        create(connection::ConnectionPtr const &conn);

        /// @brief Launch a thread running the server.
        OGVR_SERVER_EXPORT void start();

        /// @brief Launch a thread running the server, and block until the
        /// server shuts down.
        OGVR_SERVER_EXPORT void startAndAwaitShutdown();

        /// @brief Signal the server to stop (if it is running), and block until
        /// it does so.
        OGVR_SERVER_EXPORT void stop();

        /// @brief Signal the server to stop (if it is running) but return
        /// immediately.
        OGVR_SERVER_EXPORT void signalStop();

        /// @brief Load plugin by name.
        OGVR_SERVER_EXPORT void loadPlugin(std::string const &plugin);

        /// @brief Run all hardware detect callbacks.
        OGVR_SERVER_EXPORT void triggerHardwareDetect();

      private:
        unique_ptr<ServerImpl> m_impl;
    };
} // namespace server
} // namespace ogvr

#endif // INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
