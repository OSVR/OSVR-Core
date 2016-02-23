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

#ifndef INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856
#define INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856

// Internal Includes
#include <osvr/PluginHost/Export.h>
#include <osvr/PluginHost/RegistrationContext_fwd.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext_fwd.h>
#include <osvr/PluginHost/PluginRegPtr.h>
#include <osvr/Util/PluginCallbackTypesC.h>
#include <osvr/Util/PluginRegContextC.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/Util/AnyMap_fwd.h>
#include <osvr/Util/GenericDeleter.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace pluginhost {

    /// @brief Class providing the external interface of a registration context
    /// backing a single plugin
    class PluginSpecificRegistrationContext : boost::noncopyable {
      public:
        /// @brief Factory function that creates a plugin-specific registration
        /// context. Ownership is transferred to the caller.
        ///
        /// Typically called by a RegistrationContext in the loadPlugin method,
        /// this may also be used for statically-linked "plugins" whether in
        /// deployment or testing.
        ///
        /// @param name The plugin name, conventionally in an
        /// underscore-delimited reverse DNS format.
        OSVR_PLUGINHOST_EXPORT static PluginRegPtr
        create(std::string const &name);

        /// @brief Retrieve this interface from an OSVR_PluginRegContext opaque
        /// pointer.
        OSVR_PLUGINHOST_EXPORT static PluginSpecificRegistrationContext &
        get(OSVR_PluginRegContext ctx);

        /// @brief Extracts the opaque pointer for this interface to send to C.
        OSVR_PLUGINHOST_EXPORT OSVR_PluginRegContext extractOpaquePointer();

        /// @brief Destructor
        OSVR_PLUGINHOST_EXPORT virtual ~PluginSpecificRegistrationContext();

        /// @brief Get parent registration context
        ///
        /// @throws std::logic_error if called when no parent is yet set.
        OSVR_PLUGINHOST_EXPORT virtual RegistrationContext &getParent() = 0;

        /// @brief Get parent registration context
        ///
        /// @throws std::logic_error if called when no parent is yet set.
        OSVR_PLUGINHOST_EXPORT virtual RegistrationContext const &
        getParent() const = 0;

        /// @brief Access the data storage map.
        OSVR_PLUGINHOST_EXPORT virtual util::AnyMap &data() = 0;

        /// @brief Const access the data storage map.
        OSVR_PLUGINHOST_EXPORT virtual util::AnyMap const &data() const = 0;

        /// @name Plugin API
        /// @brief Called by the C API wrappers in the plugin registration
        /// headers.
        /// @{

        /// @brief Register data and a delete callback to be called on plugin
        /// unload.
        OSVR_PLUGINHOST_EXPORT virtual void registerDataWithDeleteCallback(
            OSVR_PluginDataDeleteCallback deleteCallback, void *pluginData) = 0;

        /// @brief Register data allocated with new to be deleted on plugin
        /// unload.
        template <typename T> T *registerDataWithGenericDelete(T *data) {
            registerDataWithDeleteCallback(&::osvr::util::generic_deleter<T>,
                                           static_cast<void *>(data));
            return data;
        }

        /// @brief Register a callback to be invoked on some hardware detection
        /// event.
        OSVR_PLUGINHOST_EXPORT virtual void registerHardwareDetectCallback(
            OSVR_HardwareDetectCallback detectCallback, void *userData) = 0;

        /// @brief Register a callback for constructing a driver by name with
        /// parameters.
        ///
        /// @param name Driver type name - must be non-empty and unique within
        /// this plugin.
        /// @param constructor The callback function.
        /// @param userData Optional opaque pointer to pass to callback
        ///
        /// @throws std::logic_error if name is empty or already used within
        /// this plugin.
        OSVR_PLUGINHOST_EXPORT virtual void registerDriverInstantiationCallback(
            const char *name, OSVR_DriverInstantiationCallback constructor,
            void *userData) = 0;
        /// @}

        /// @brief Accessor for plugin name.
        OSVR_PLUGINHOST_EXPORT const std::string &getName() const;

      protected:
        /// @brief Constructor for derived class use only
        PluginSpecificRegistrationContext(std::string const &name);

      private:
        std::string const m_name;
    };
} // namespace pluginhost
} // namespace osvr
#endif // INCLUDED_PluginSpecificRegistrationContext_h_GUID_8C008527_0BF6_408F_3C73_4FE76B77D856
