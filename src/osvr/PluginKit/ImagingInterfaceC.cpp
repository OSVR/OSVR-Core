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
#include <osvr/PluginKit/ImagingInterfaceC.h>
#include <osvr/Connection/DeviceInitObject.h>
//#include <osvr/Connection/ImagingServerInterface.h>
#include <osvr/Connection/DeviceToken.h>
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>
#include "PointerWrapper.h"
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    class ImagingServerInterface;
}
}

struct OSVR_ImagingDeviceInterfaceObject
    : public PointerWrapper<osvr::connection::ImagingServerInterface> {};

OSVR_ReturnCode
osvrDeviceImagingConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                           OSVR_OUT_PTR OSVR_ImagingDeviceInterface *iface,
                           OSVR_IN OSVR_ChannelCount numSensors) {

    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceImagingConfigure", opts);
    OSVR_PLUGIN_HANDLE_NULL_CONTEXT("osvrDeviceImagingConfigure", iface);
    OSVR_ImagingDeviceInterface ifaceObj =
        opts->getContext()->registerDataWithGenericDelete(
            new OSVR_ImagingDeviceInterfaceObject);
    *iface = ifaceObj;
    /// @todo update the opts object to add the new interface
    /// opts->setAnalogs(numChan, ifaceObj->getContainerLocation());
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceImagingReportFrame(OSVR_IN_PTR OSVR_DeviceToken dev,
                             OSVR_IN_PTR OSVR_ImagingDeviceInterface iface,
                             OSVR_IN OSVR_ImagingMetadata metadata,
                             OSVR_IN_PTR void *imageData,
                             OSVR_IN OSVR_ChannelCount sensor,
                             OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = dev->getSendGuard();
    if (guard->lock()) {
        bool sendResult =
            true; /// @todo (*iface)->setValue(val, chan, *timestamp);
        OSVR_DEV_VERBOSE("Sending image report " << metadata.width << "x"
                                                 << metadata.height);
        return sendResult ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
    }

    return OSVR_RETURN_FAILURE;
}