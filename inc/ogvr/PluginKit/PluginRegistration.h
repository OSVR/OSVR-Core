/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
#define INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9

// Internal Includes
#include <ogvr/PluginKit/PluginRegistrationC.h>
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>
#include <string>

namespace ogvr {
namespace plugin {
    /// @brief Registers an object to be destroyed with delete when the plugin
    /// is unloaded.
    ///
    /// The template parameter does not need to be specified - it is deduced
    /// automatically.
    ///
    /// The plugin remains nominally the owner of the object, but your code
    /// should no longer concern itself with the destruction of this object.
    /// Thus, if you have it in a smart pointer of any kind, release it from
    /// that pointer when you pass it to your function.
    ///
    /// @param ctx The registration context passed to your entry point.
    /// @param obj A pointer to your object to register for deletion.
    ///
    /// Internally uses ogvrPluginRegisterDataWithDeleteCallback()
    template <typename T>
    inline OGVR_PluginReturnCode
    registerObjectForDeletion(OGVR_PluginRegContext ctx, T *obj) {
        return ogvrPluginRegisterDataWithDeleteCallback(
            ctx, &detail::generic_deleter<T>, static_cast<void *>(obj));
    }
} // end of namespace plugin
} // end of namespace ogvr

#endif // INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
