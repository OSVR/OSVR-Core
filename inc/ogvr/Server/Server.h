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

#ifndef INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
#define INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367

// Internal Includes
#include <ogvr/Server/Export.h>
#include <ogvr/Server/ServerPtr.h>
#include <ogvr/Connection/ConnectionPtr.h>
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginHost/RegistrationContext_fwd.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace server {
    class Server {
      public:
        /// @brief Server constructor with an existing Connection
        /// @throws std::logic_error if a null connection is passed.
        OGVR_SERVER_EXPORT Server(connection::ConnectionPtr conn);

        /// @brief Create a server object with a local-only connection.
        OGVR_SERVER_EXPORT static ServerPtr createLocal();

      private:
        connection::ConnectionPtr m_conn;
        shared_ptr<pluginhost::RegistrationContext> m_ctx;
    };
} // namespace server
} // namespace ogvr

#endif // INCLUDED_Server_h_GUID_453158B3_45B7_478D_FB83_F2F694CA5367
