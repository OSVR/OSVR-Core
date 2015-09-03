/** @file
    @brief Header including the full PluginKit C++ interface.

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

#ifndef INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89
#define INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89

// Internal Includes
#include <osvr/PluginKit/DeviceInterface.h>
#include <osvr/PluginKit/PluginRegistration.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {

/** @brief Contains C++ header-only wrappers for the PluginKit C API.
    @ingroup PluginKit

    Use of these wrappers is optional: they duplicate functionality found in the
    C API as they are implemented in headers on top of the C API. However, they
    also provide a much cleaner, low-boilerplate interface for the most common
    operations.

    The free functions and the members of osvr::pluginkit::PluginContext perform
    the same functionality. Use of the class can just lead to cleaner code if
    your whole plugin is "C++-style".
*/
namespace pluginkit {
    /// @brief C++ wrapper class for the opaque plugin context.
    ///
    /// This class provides no additional functionality beyond the free
    /// functions available in this namespace, it just provides syntactic
    /// sugar.
    class PluginContext {
      public:
        /// @brief Constructor of the context wrapper object
        PluginContext(OSVR_PluginRegContext ctx) : m_ctx(ctx) {}

        /// @brief Register a hardware detect callback
        ///
        /// Your callback should take one parameter of type
        /// ::OSVR_PluginRegContext and return a value of type ::OSVR_ReturnCode
        ///
        /// @sa ::osvr::pluginkit::registerHardwareDetectCallback()
        template <typename T> void registerHardwareDetectCallback(T functor) {
            ::osvr::pluginkit::registerHardwareDetectCallback(m_ctx, functor);
        }

        /// @brief Register a driver instantiation callback
        ///
        /// Your callback should take a parameter of type
        /// ::OSVR_PluginRegContext and a parameter of type const char * (the
        /// JSON parameters as a string) and return a value of type
        /// ::OSVR_ReturnCode
        ///
        /// @sa ::osvr::pluginkit::registerDriverInstantiationCallback()
        template <typename T>
        void registerDriverInstantiationCallback(const char driverName[],
                                                 T functor) {
            ::osvr::pluginkit::registerDriverInstantiationCallback(
                m_ctx, driverName, functor);
        }

        /// @brief Register the given object (assumed to be deletable by
        /// `delete`) to be deleted on plugin unload. (Transfers lifetime
        /// control to the plugin context)
        /// @sa ::osvr::pluginkit::registerObjectForDeletion
        template <typename T> T *registerObjectForDeletion(T *obj) {
            return ::osvr::pluginkit::registerObjectForDeletion(m_ctx, obj);
        }

      private:
        OSVR_PluginRegContext m_ctx;
    };

} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89
