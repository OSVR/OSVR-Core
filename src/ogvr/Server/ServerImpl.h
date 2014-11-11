/** @file
    @brief Header

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

#ifndef INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982
#define INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982

// Internal Includes
#include <ogvr/Server/Server.h>
#include <ogvr/Connection/ConnectionPtr.h>
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginHost/RegistrationContext_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <util/RunLoopManagerBoost.h>
#include <boost/thread.hpp>

// Standard includes
// - none

namespace ogvr {
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

        /// @brief Signal the server to stop, and block until it does so.
        void stop();

        /// @brief The method called from the server thread repeatedly.
        /// @returns true if the loop should continue running
        bool loop();

      private:
        connection::ConnectionPtr m_conn;
        shared_ptr<pluginhost::RegistrationContext> m_ctx;

        boost::thread m_thread;
        ::util::RunLoopManagerBoost m_run;
    };

} // namespace server
} // namespace ogvr

#endif // INCLUDED_ServerImpl_h_GUID_BA15589C_D1AD_4BBE_4F93_8AC87043A982
