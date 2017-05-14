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

#ifndef INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
#define INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0

// Internal Includes
#include <osvr/PluginKit/PluginKitC.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/CallbackWrapper.h>
#include <osvr/Util/AnyMap.h>

// Library/third-party includes
#include <libfunctionality/PluginHandle.h>

// Standard includes
#include <vector>
#include <functional>

namespace osvr {

namespace pluginhost {
    /// @brief Internal class backing the context of registrations performed by
    /// a
    /// single plugin.
    class PluginSpecificRegistrationContextImpl
        : public PluginSpecificRegistrationContext {
      public:
        /// @brief Constructor
        PluginSpecificRegistrationContextImpl(std::string const &name);

        /// @brief Destructor
        ///
        /// Responsible for destroying plugin data in reverse order.
        ~PluginSpecificRegistrationContextImpl();

        /// @brief Assume ownership of the plugin handle keeping the plugin
        /// library
        /// loaded.
        void takePluginHandle(libfunc::PluginHandle &handle);

        /// @brief Set parent registration context
        ///
        /// Should usually called only by RegistrationContext, and only once.
        /// If called multiple times with the same parent, this is OK.
        ///
        /// @throws std::logic_error if called when a different parent is
        /// already
        /// set.
        void setParent(RegistrationContext &parent);

        /// @brief Get parent registration context
        ///
        /// @throws std::logic_error if called when no parent is yet set.
        virtual RegistrationContext &getParent();

        /// @brief Get parent registration context
        ///
        /// @throws std::logic_error if called when no parent is yet set.
        virtual RegistrationContext const &getParent() const;

        /// @brief Call all hardware detect callbacks registered by this plugin,
        /// if any.
        void triggerHardwareDetectCallbacks();

        /// @brief Call a driver instantiation callback for the given driver
        /// name.
        /// @throws std::runtime_error if there is no driver registered by that
        /// name in the given plugin, or if the callback returns failure.
        void instantiateDriver(const std::string &driverName,
                               const std::string &params = std::string()) const;

        /// @brief Access the data storage map.
        virtual util::AnyMap &data();

        /// @brief Const access the data storage map.
        virtual util::AnyMap const &data() const;

        /// @name Plugin API
        /// @brief Called by the C API wrappers in the plugin registration
        /// headers.
        /// @{

        /// @brief Register data and a delete callback to be called on plugin
        /// unload.
        virtual void registerDataWithDeleteCallback(
            OSVR_PluginDataDeleteCallback deleteCallback, void *pluginData);

        virtual void registerHardwareDetectCallback(
            OSVR_HardwareDetectCallback detectCallback, void *userData);
        virtual void registerDriverInstantiationCallback(
            const char *name, OSVR_DriverInstantiationCallback constructor,
            void *userData);
        /// @}

        /// @brief Trigger system-wide hardware detection.
        virtual void triggerHardwareDetect();

      private:
        /// @brief Pointer with ownership semantics for deletion of plugin data.
        typedef unique_ptr<void, OSVR_PluginDataDeleteCallback> PluginDataPtr;
        /// @brief List of plugin data.
        typedef std::vector<PluginDataPtr> PluginDataList;

        PluginDataList m_dataList;
        libfunc::PluginHandle m_handle;
        RegistrationContext *m_parent;

        typedef util::CallbackWrapper<OSVR_HardwareDetectCallback>
            HardwareDetectCallback;
        typedef std::vector<HardwareDetectCallback> HardwareDetectCallbackList;
        HardwareDetectCallbackList m_hardwareDetectCallbacks;

        typedef std::function<OSVR_ReturnCode(const char *)>
            DriverInstantiationCallback;
        typedef std::map<std::string, DriverInstantiationCallback>
            DriverInstantiationMap;
        DriverInstantiationMap m_driverInstantiationCallbacks;

        util::AnyMap m_data;
    };

} // namespace pluginhost
} // namespace osvr

#endif // INCLUDED_PluginSpecificRegistrationContextImpl_h_GUID_E8A348C1_28DC_4691_6214_32F75A6665F0
