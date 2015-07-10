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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NormalizeDeviceDescriptor_h_GUID_1DF9C796_7C76_47A2_74DA_5DDCF1F407C9
#define INCLUDED_NormalizeDeviceDescriptor_h_GUID_1DF9C796_7C76_47A2_74DA_5DDCF1F407C9

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief "Normalizes" a device descriptor by parsing it and adding implied
    /// interfaces to the existing device descriptor before serializing it back
    /// out to a string. Contains logic for every interface that combines
    /// multiple other interfaces, such as eyetracker (direction, location2d,
    /// tracker, button)
    ///
    /// @return a serialized JSON string (may be unmodified)
    OSVR_COMMON_EXPORT std::string
    normalizeDeviceDescriptor(std::string const &jsonDescriptor);

} // namespace common
} // namespace osvr

#endif // INCLUDED_NormalizeDeviceDescriptor_h_GUID_1DF9C796_7C76_47A2_74DA_5DDCF1F407C9
