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
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>
#include "GetJSONStringFromTree.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>

using ::osvr::common::ClientInterfacePtr;
using ::osvr::common::ClientInterface;
using ::osvr::common::ClientContextDeleter;
using ::osvr::make_shared;

namespace osvr {
namespace common {
    void deleteContext(ClientContext *ctx) {
        auto del = ctx->getDeleter();
        (*del)(ctx);
    }
} // namespace common
} // namespace osvr
OSVR_ClientContextObject::OSVR_ClientContextObject(const char appId[],
                                                   ClientContextDeleter del)
    : m_appId(appId), m_deleter(del) {
    OSVR_DEV_VERBOSE("Client context initialized for " << m_appId);
}

OSVR_ClientContextObject::~OSVR_ClientContextObject() {
    OSVR_DEV_VERBOSE("Client context shut down for " << m_appId);
}

std::string const &OSVR_ClientContextObject::getAppId() const {
    return m_appId;
}

void OSVR_ClientContextObject::update() {
    m_update();
    for (auto const &iface : m_interfaces) {
        iface->update();
    }
}

ClientInterfacePtr OSVR_ClientContextObject::getInterface(const char path[]) {
    ClientInterfacePtr ret;
    if (!path) {
        return ret;
    }
    std::string p(path);
    if (p.empty()) {
        return ret;
    }
    ret = make_shared<ClientInterface>(this, path,
                                       ClientInterface::PrivateConstructor());
    m_handleNewInterface(ret);
    m_interfaces.push_back(ret);
    return ret;
}

ClientInterfacePtr
OSVR_ClientContextObject::releaseInterface(ClientInterface *iface) {
    ClientInterfacePtr ret;
    if (!iface) {
        return ret;
    }
    auto it = std::find_if(begin(m_interfaces), end(m_interfaces),
                           [&](ClientInterfacePtr const &ptr) {
                               if (ptr.get() == iface) {
                                   ret = ptr;
                                   return true;
                               }
                               return false;
                           });
    BOOST_ASSERT_MSG(
        (it == end(m_interfaces)) == (!ret),
        "We should have a pointer if and only if we have the iterator");
    if (ret) {
        // Erase it from our list
        m_interfaces.erase(it);
        // Notify the derived class if desired
        m_handleReleasingInterface(ret);
    }
    return ret;
}

std::string
OSVR_ClientContextObject::getStringParameter(std::string const &path) const {
    return getJSONStringFromTree(getPathTree(), path);
}

osvr::common::PathTree const &OSVR_ClientContextObject::getPathTree() const {
    return m_getPathTree();
}

void OSVR_ClientContextObject::sendRoute(std::string const &route) {
    m_sendRoute(route);
}

bool OSVR_ClientContextObject::releaseObject(void *obj) {
    return m_ownedObjects.release(obj);
}

ClientContextDeleter OSVR_ClientContextObject::getDeleter() const {
    return m_deleter;
}
bool OSVR_ClientContextObject::getStatus() const { return m_getStatus(); }
bool OSVR_ClientContextObject::m_getStatus() const {
    // by default, assume we are started up.
    return true;
}
void OSVR_ClientContextObject::m_handleNewInterface(
    ::osvr::common::ClientInterfacePtr const &) {
    // by default do nothing
}
void OSVR_ClientContextObject::m_handleReleasingInterface(
    ::osvr::common::ClientInterfacePtr const &) {
    // by default do nothing
}
