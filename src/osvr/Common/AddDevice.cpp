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

// Internal Includes
#include <osvr/Common/AddDevice.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Common/RoutingExceptions.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace osvr {
namespace common {
    PathNode &addDevice(PathTree &tree, std::string const &deviceName) {
        if (deviceName.size() < 3) { // Minimum size: a/b
            throw exceptions::InvalidDeviceName("Given device name is too "
                                                "short to be a complete device "
                                                "name: " +
                                                deviceName);
        }
        std::string normalized = (deviceName[0] == getPathSeparatorCharacter())
                                     ? (deviceName)
                                     : (getPathSeparator() + deviceName);
        PathNode &device = tree.getNodeByPath(normalized);
        auto plugin = device.getParent();
        if (plugin == nullptr || plugin->isRoot()) {
            /// @todo remove added node here?
            throw exceptions::InvalidDeviceName(
                "Given device name did not include both a "
                "plugin component and a device "
                "component: " +
                deviceName);
        }
        // Might be several layers before we hit the plugin - get the last
        // parent before the root.
        /// @todo What about intermediate paths between a plugin and a device?
        /// (device types, for instance) We leave them null for now.
        while (plugin->getParent() != nullptr &&
               !plugin->getParent()->isRoot()) {
            plugin = plugin->getParent();
        }
        elements::ifNullReplaceWith(device.value(), elements::DeviceElement());
        elements::ifNullReplaceWith(plugin->value(), elements::PluginElement());
        return device;
    }
} // namespace common
} // namespace osvr
