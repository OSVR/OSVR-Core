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

#define OSVR_DEV_VERBOSE_DISABLE

// Internal Includes
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/PluginHost/RegistrationContext.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/range/adaptor/reversed.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace pluginhost {
    PluginSpecificRegistrationContextImpl::
        PluginSpecificRegistrationContextImpl(std::string const &name)
        : PluginSpecificRegistrationContext(name), m_parent(nullptr) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         << "Creating a plugin registration context for "
                         << name);
    }

    PluginSpecificRegistrationContextImpl::
        ~PluginSpecificRegistrationContextImpl() {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContextImpl:\t"
                         "Destroying plugin reg context for "
                         << getName());

        // Delete the data in reverse order.
        for (auto &ptr : m_dataList | boost::adaptors::reversed) {
            ptr.reset();
        }
        m_parent = nullptr; // before anything else destructs, for safety?
    }

    void PluginSpecificRegistrationContextImpl::takePluginHandle(
        libfunc::PluginHandle &handle) {
        m_handle = handle;
    }

    void PluginSpecificRegistrationContextImpl::setParent(
        RegistrationContext &parent) {
        if (m_parent != nullptr && m_parent != &parent) {
            throw std::logic_error(
                "Can't set the registration context parent - already set!");
        }
        m_parent = &parent;
    }

    RegistrationContext &PluginSpecificRegistrationContextImpl::getParent() {
        if (m_parent == nullptr) {
            throw std::logic_error(
                "Can't access the registration context parent - it is null!");
        }
        return *m_parent;
    }

    RegistrationContext const &
    PluginSpecificRegistrationContextImpl::getParent() const {
        if (m_parent == nullptr) {
            throw std::logic_error(
                "Can't access the registration context parent - it is null!");
        }
        return *m_parent;
    }

    void
    PluginSpecificRegistrationContextImpl::triggerHardwareDetectCallbacks() {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In triggerHardwareDetectCallbacks for "
                         << getName());

        for (auto const &f : m_hardwareDetectCallbacks) {
            f(this);
        }
    }

    void PluginSpecificRegistrationContextImpl::instantiateDriver(
        const std::string &driverName, const std::string &params) const {
        auto it = m_driverInstantiationCallbacks.find(driverName);
        if (it == end(m_driverInstantiationCallbacks)) {
            throw std::logic_error("No driver initialization callback was "
                                   "registered for the driver name " +
                                   driverName);
        }
        OSVR_ReturnCode ret = (it->second)(params.c_str());
        if (ret != OSVR_RETURN_SUCCESS) {
            throw std::runtime_error("Failure returned from driver "
                                     "initialization callback by name " +
                                     driverName);
        }
    }

    void PluginSpecificRegistrationContextImpl::registerDataWithDeleteCallback(
        OSVR_PluginDataDeleteCallback deleteCallback, void *pluginData) {
        m_dataList.emplace_back(pluginData, deleteCallback);
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_dataList.size()
                         << " data delete callbacks registered for "
                         << getName());
    }

    void PluginSpecificRegistrationContextImpl::registerHardwareDetectCallback(
        OSVR_HardwareDetectCallback detectCallback, void *userData) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In registerHardwareDetectCallback");
        m_hardwareDetectCallbacks.emplace_back(detectCallback, userData);
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "Now have "
                         << m_hardwareDetectCallbacks.size()
                         << " hardware detect callbacks registered for "
                         << getName());
    }

    void
    PluginSpecificRegistrationContextImpl::registerDriverInstantiationCallback(
        const char *name, OSVR_DriverInstantiationCallback constructor,
        void *userData) {
        OSVR_DEV_VERBOSE("PluginSpecificRegistrationContext:\t"
                         "In registerDriverInstantiationCallback");
        std::string n(name);
        if (n.empty()) {
            throw std::logic_error("Cannot register a driver instantiation "
                                   "callback with an empty name!");
        }
        using namespace std::placeholders;
        auto it = m_driverInstantiationCallbacks.find(name);
        if (it != end(m_driverInstantiationCallbacks)) {
            throw std::logic_error("A driver initialization callback by this "
                                   "name for this plugin has already been "
                                   "registered!");
        }
        auto opaque = extractOpaquePointer();
        m_driverInstantiationCallbacks[name] =
            [constructor, opaque, userData](const char *params) {
                return constructor(opaque, params, userData);
            };
    }

    util::AnyMap &PluginSpecificRegistrationContextImpl::data() {
        return m_data;
    }

    util::AnyMap const &PluginSpecificRegistrationContextImpl::data() const {
        return m_data;
    }

    void PluginSpecificRegistrationContextImpl::triggerHardwareDetect() {
        if (m_parent == nullptr) {
            throw std::logic_error(
                "Can't access the registration context parent - it is null!");
        }
        m_parent->triggerHardwareDetect();
    }

} // namespace pluginhost
} // namespace osvr
