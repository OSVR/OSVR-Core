/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96
#define INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/BaseDevicePtr.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Factory function for a bare client device with no
    /// components/interfaces registered by default
    OSVR_COMMON_EXPORT BaseDevicePtr
    createClientDevice(std::string const &name, vrpn_ConnectionPtr const &conn);
    /// @brief Factory function for a bare server device with no
    /// components/interfaces registered by default
    OSVR_COMMON_EXPORT BaseDevicePtr
    createServerDevice(std::string const &name, vrpn_ConnectionPtr const &conn);
} // namespace common
} // namespace osvr

#endif // INCLUDED_CreateDevice_h_GUID_139C0CB3_5AC9_4C83_A9CC_A282BEE93F96
