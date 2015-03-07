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

#ifndef INCLUDED_AddDevice_h_GUID_DEA28469_5227_44AD_1972_94548FA58E22
#define INCLUDED_AddDevice_h_GUID_DEA28469_5227_44AD_1972_94548FA58E22

// Internal Includes
#include <osvr/Routing/Export.h>
#include <osvr/Routing/PathTree_fwd.h>
#include <osvr/Routing/PathNode_fwd.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

namespace osvr {
namespace routing {
    /// @brief Adds/updates nodes for the basic path to a device.
    ///
    /// @ingroup Routing
    ///
    /// @param tree Your path tree.
    /// @param deviceName A namespaced device name coming from a plugin,
    /// like `/org_opengoggles_plugin/SampleDevice`. If a leading slash is
    /// missing, it will be assumed and added.
    ///
    /// Multiple levels are acceptable: for example,
    /// `/org_opengoggles_plugin/SampleDriver/Instance1` is valid: Instance1
    /// is the device, and org_opengoggles_plugin is the plugin.
    ///
    /// @returns The device node
    /// @throws osvr::routing::exceptions::InvalidDeviceName if an invalid
    /// device name (less than two components) was passed
    /// @throws osvr::routing::exceptions::EmptyPathComponent if any component
    /// in the given path is empty after passing initial checks.
    OSVR_ROUTING_EXPORT PathNode &addDevice(PathTree &tree,
                                            std::string const &deviceName);
} // namespace routing
} // namespace osvr

#endif // INCLUDED_AddDevice_h_GUID_DEA28469_5227_44AD_1972_94548FA58E22
