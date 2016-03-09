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

#ifndef INCLUDED_ProcessDeviceDescriptor_h_GUID_BB1EC60E_95C8_4C96_3FF3_F16B1C83A5A1
#define INCLUDED_ProcessDeviceDescriptor_h_GUID_BB1EC60E_95C8_4C96_3FF3_F16B1C83A5A1

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathElementTypes_fwd.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace common {
    /// @brief Set up a path tree based on a device descriptor
    ///
    /// @return true if changes were made
    OSVR_COMMON_EXPORT bool
    processDeviceDescriptorForPathTree(PathTree &tree,
                                       std::string const &deviceName,
                                       std::string const &jsonDescriptor,
                                       int listenPort, std::string const &host);

    /// @brief Set up a path tree based on a device descriptor from an existing
    /// DeviceElement node
    ///
    /// @return true if changes were made
    OSVR_COMMON_EXPORT bool processDeviceDescriptorFromExistingDevice(
        PathNode &devNode, elements::DeviceElement const &devElt);

} // namespace common
} // namespace osvr

#endif // INCLUDED_ProcessDeviceDescriptor_h_GUID_BB1EC60E_95C8_4C96_3FF3_F16B1C83A5A1
