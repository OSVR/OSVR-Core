/** @file
    @brief Implementation

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

// Internal Includes
#include <ogvr/Server/Server.h>
#include <ogvr/Connection/Connection.h>
#include <ogvr/PluginHost/RegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace ogvr {
namespace server {

    ServerPtr Server::createLocal() {
        ConnectionPtr conn(Connection::createLocalConnection());
        ServerPtr ret(make_shared<Server>(conn));
        return ret;
    }

    Server::Server(ConnectionPtr conn)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        ogvr::Connection::storeConnection(*m_ctx, m_conn);
    }

} // end of namespace server
} // end of namespace ogvr