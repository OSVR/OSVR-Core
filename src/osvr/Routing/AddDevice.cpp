/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Routing/AddDevice.h>
#include <osvr/Routing/PathNode.h>
#include <osvr/Routing/PathTree.h>
#include <osvr/Routing/PathElementTools.h>
#include <osvr/Routing/Constants.h>
#include <osvr/Routing/Exceptions.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

namespace osvr {
namespace routing {
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
        PathNodePtr plugin = device.getParent();
        if (!plugin || plugin->isRoot()) {
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
        while (plugin->getParent() && !plugin->getParent()->isRoot()) {
            plugin = plugin->getParent();
        }
        elements::ifNullReplaceWith(device.value(), elements::DeviceElement());
        elements::ifNullReplaceWith(plugin->value(), elements::PluginElement());
        return device;
    }
} // namespace routing
} // namespace osvr