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
#include <osvr/Connection/Connection.h>
#include <osvr/PluginHost/RegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <functional>

namespace osvr {
namespace server {
    ServerImpl::ServerImpl(connection::ConnectionPtr const &conn)
        : m_conn(conn), m_ctx(make_shared<pluginhost::RegistrationContext>()),
          m_running(false) {
        if (!m_conn) {
            throw std::logic_error(
                "Can't pass a null ConnectionPtr into Server constructor!");
        }
        osvr::connection::Connection::storeConnection(*m_ctx, m_conn);
    }

    ServerImpl::~ServerImpl() { stop(); }

    void ServerImpl::start() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_running = true;

        // Use a lambda to run the loop.
        m_thread = boost::thread([&] {
            bool keepRunning = true;
            ::util::LoopGuard guard(m_run);
            do {
                keepRunning = this->loop();
            } while (keepRunning);
            m_ctx.reset();
            m_conn.reset();
            m_running = false;
        });
        m_run.signalAndWaitForStart();
    }

    void ServerImpl::startAndAwaitShutdown() {
        start();
        m_thread.join();
    }

    void ServerImpl::stop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalAndWaitForShutdown();
        m_thread.join();
        m_thread = boost::thread();
    }
    void ServerImpl::signalStop() {
        boost::unique_lock<boost::mutex> lock(m_runControl);
        m_run.signalShutdown();
    }

    void ServerImpl::loadPlugin(std::string const &pluginName) {
        m_callControlled(std::bind(&pluginhost::RegistrationContext::loadPlugin,
                                   m_ctx, pluginName));
    }

    void ServerImpl::triggerHardwareDetect() {
        m_callControlled(std::bind(
            &pluginhost::RegistrationContext::triggerHardwareDetect, m_ctx));
    }

    bool ServerImpl::loop() {
        m_conn->process();
        /// @todo do queued things in here?
        /// @todo configurable waiting?
        m_thread.yield();
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
} // namespace osvr