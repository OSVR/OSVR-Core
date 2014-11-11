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
#include <functional>

namespace ogvr {
namespace server {
    ServerImpl::ServerImpl(connection::ConnectionPtr const &conn)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()),
          m_running(false) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        ogvr::connection::Connection::storeConnection(*m_ctx, m_conn);
    }

    ServerImpl::~ServerImpl() { stop(); }

    void ServerImpl::start() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_running = true;
        // Get a reference that we can capture in the lambda.
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

    void ServerImpl::stop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalAndWaitForShutdown();
        m_thread.join();
        m_thread = boost::thread();
        m_running = false;
    }

    void ServerImpl::loadPlugin(std::string const &pluginName) {
        m_callControlled(std::bind(&pluginhost::RegistrationContext::loadPlugin,
                                   m_ctx, pluginName));
    }

    void ServerImpl::triggerHardwarePoll() {
        m_callControlled(std::bind(
            &pluginhost::RegistrationContext::triggerHardwarePoll, m_ctx));
    }

    bool ServerImpl::loop() {
        m_conn->process();
        /// @todo do queued things in here?
        return m_run.shouldContinue();
    }

    template <typename Callable>
    inline void ServerImpl::m_callControlled(Callable f) {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        if (m_running) {
            /// @todo not yet implemented
            throw std::logic_error("not yet implemented");
        }
        f();
    }

} // namespace server
} // namespace ogvr