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
#include "ServerImpl.h"
#include <ogvr/Connection/Connection.h>
#include <ogvr/PluginHost/RegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace ogvr {
namespace server {
    ServerImpl::ServerImpl(connection::ConnectionPtr const &conn)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        ogvr::connection::Connection::storeConnection(*m_ctx, m_conn);
    }

    ServerImpl::~ServerImpl() { stop(); }

    void ServerImpl::start() {
        ::util::LoopGuardInterface &run(m_run);
        // Use a lambda to run the loop.
        m_thread = boost::thread([&] {
            bool keepRunning = true;
            ::util::LoopGuard guard(run);
            do {
                keepRunning = this->loop();
            } while (keepRunning);
        });
        m_run.signalAndWaitForStart();
    }

    void ServerImpl::stop() { m_run.signalAndWaitForShutdown(); }

    bool ServerImpl::loop() {

        /// @todo do things in here.
        return m_run.shouldContinue();
    }

} // namespace server
} // namespace ogvr