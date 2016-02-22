/** @file
    @brief Header

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

#ifndef INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
#define INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10

// Internal Includes
#include <osvr/PluginHost/RegistrationContext_fwd.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/AnyMap.h>
#include <osvr/PluginHost/Export.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <map>

namespace osvr {
/// @brief PluginHost functionality: loading, hosting, registering, destroying,
/// and enumerating plugins.
/// @ingroup PluginHost
namespace pluginhost {

    /// @brief Class responsible for hosting plugins, along with their
    /// registration and destruction
    class RegistrationContext : boost::noncopyable {
      public:
        /// @brief basic constructor
        OSVR_PLUGINHOST_EXPORT RegistrationContext();

        /// @brief Destructor responsible for destroying plugins in reverse
        /// order.
        OSVR_PLUGINHOST_EXPORT ~RegistrationContext();

        /// @name Host-side (internal) API
        /// @{
        /// @brief Load a plugin from a dynamic library in this context
        OSVR_PLUGINHOST_EXPORT void loadPlugin(std::string const &pluginName);

        /// @brief Load all detected plugins except those with a .manualload
        /// suffix
        OSVR_PLUGINHOST_EXPORT void loadPlugins();

        /// @brief Assume ownership of a plugin-specific registration context
        /// created and initialized outside of loadPlugin.
        OSVR_PLUGINHOST_EXPORT void
        adoptPluginRegistrationContext(PluginRegPtr ctx);

        /// @brief Trigger any registered hardware detect callbacks.
        OSVR_PLUGINHOST_EXPORT void triggerHardwareDetect();

        /// @brief Call a driver instantiation callback for the given plugin
        /// name and driver name.
        /// @throws std::runtime_error if the plugin named hasn't been loaded,
        /// if there is no driver registered by that name in the given plugin,
        /// or if the constructor returns failure.
        OSVR_PLUGINHOST_EXPORT void
        instantiateDriver(const std::string &pluginName,
                          const std::string &driverName,
                          const std::string &params = std::string()) const;

        /// @brief Access the data storage map.
        OSVR_PLUGINHOST_EXPORT util::AnyMap &data();

        /// @brief Const access the data storage map.
        OSVR_PLUGINHOST_EXPORT util::AnyMap const &data() const;
        /// @}

      private:
        /// @brief Map of plugin names to owning pointers for plugin
        /// registration.
        typedef std::map<std::string, PluginRegPtr> PluginRegMap;

        PluginRegMap m_regMap;
        util::AnyMap m_data;
        struct Impl;
        /// Private impl.
        unique_ptr<Impl> m_impl;
    };
} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
