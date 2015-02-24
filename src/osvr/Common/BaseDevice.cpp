/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/DeviceComponent.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace common {

    BaseDevice::BaseDevice() {}
    BaseDevice::~BaseDevice() {}

    /// @brief For adding components to a base device.
    void BaseDevice::addComponent(DeviceComponentPtr component) {
        if (!component) {
            throw std::logic_error(
                "Tried to add a null component pointer to a base device!");
        }
        component->recordParent(*this);
    }

} // namespace common
} // namespace osvr