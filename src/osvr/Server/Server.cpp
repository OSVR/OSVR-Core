/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Server/Server.h>
#include <osvr/Connection/Connection.h>
#include "ServerImpl.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace server {

    ServerPtr Server::createLocal() {
        connection::ConnectionPtr conn(
            connection::Connection::createLocalConnection());
        ServerPtr ret(make_shared<Server>(conn, private_constructor{}));
        return ret;
    }

    ServerPtr Server::create(connection::ConnectionPtr const &conn) {
        ServerPtr ret(make_shared<Server>(conn, private_constructor{}));
        return ret;
    }

    void Server::update() { m_impl->update(); }

    void Server::start() { m_impl->start(); }

    void Server::awaitShutdown() { m_impl->awaitShutdown(); }

    void Server::startAndAwaitShutdown() { m_impl->startAndAwaitShutdown(); }

    void Server::stop() { m_impl->stop(); }

    void Server::signalStop() { m_impl->signalStop(); }

    void Server::loadPlugin(std::string const &plugin) {
        m_impl->loadPlugin(plugin);
    }

    void Server::loadAutoPlugins() { m_impl->loadAutoPlugins(); }

    void Server::setHardwareDetectOnConnection() {
        m_impl->setHardwareDetectOnConnection();
    }

    void Server::instantiateDriver(std::string const &plugin,
                                   std::string const &driver,
                                   std::string const &params) {
        m_impl->instantiateDriver(plugin, driver, params);
    }

    void Server::triggerHardwareDetect() { m_impl->triggerHardwareDetect(); }

    void Server::registerMainloopMethod(MainloopMethod f) {
        m_impl->registerMainloopMethod(f);
    }

    bool Server::addRoute(std::string const &routingDirective) {
        return m_impl->addRoute(routingDirective);
    }

    bool Server::addAlias(std::string const &path, std::string const &source,
                          common::AliasPriority priority) {
        return m_impl->addAlias(path, source, priority);
    }

    bool Server::addString(std::string const &path, std::string const &value) {
        return m_impl->addString(path, value);
    }

    bool Server::addAliases(Json::Value const &aliases,
                            common::AliasPriority priority) {
        return m_impl->addAliases(aliases, priority);
    }

    void Server::addExternalDevice(std::string const &path,
                                   std::string const &deviceName,
                                   std::string const &server,
                                   std::string const &descriptor) {
        m_impl->addExternalDevice(path, deviceName, server, descriptor);
    }

    std::string Server::getSource(std::string const &destination) const {
        return m_impl->getSource(destination);
    }

    void Server::setSleepTime(int microseconds) {
        m_impl->setSleepTime(microseconds);
    }

    int Server::getSleepTime() const { return m_impl->getSleepTime(); }

    Server::Server(connection::ConnectionPtr const &conn,
                   private_constructor const &)
        : m_impl(new ServerImpl(conn)) {}

    Server::~Server() {}

} // namespace server
} // namespace osvr
