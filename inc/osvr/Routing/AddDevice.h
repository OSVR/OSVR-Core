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
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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
    /// @param tree Your path tree.
    /// @param deviceName A namespaced device name coming from a plugin,
    /// like "/org_opengoggles_plugin/SampleDevice". If a leading slash is
    /// missing, it will be assumed and added.
    ///
    /// @returns The device node
    /// @throws std::runtime_error if an invalid device name (less than two
    /// components) was passed.
    OSVR_ROUTING_EXPORT PathNode &addDevice(PathTree &tree,
                                            std::string const &deviceName);
} // namespace routing
} // namespace osvr

#endif // INCLUDED_AddDevice_h_GUID_DEA28469_5227_44AD_1972_94548FA58E22
