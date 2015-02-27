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
#include <osvr/Common/ImagingComponent.h>
#include "PointerWrapper.h"
#include "HandleNullContext.h"
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_ImagingDeviceInterfaceObject {
    osvr::common::ImagingComponent *imaging;
};

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
    auto imaging = osvr::common::ImagingComponent::create(numSensors);
    ifaceObj->imaging = imaging.get();
    opts->addComponent(imaging);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode
osvrDeviceImagingReportFrame(OSVR_IN_PTR OSVR_DeviceToken dev,
                             OSVR_IN_PTR OSVR_ImagingDeviceInterface iface,
                             OSVR_IN OSVR_ImagingMetadata metadata,
                             OSVR_IN_PTR OSVR_ImageBufferElement *imageData,
                             OSVR_IN OSVR_ChannelCount sensor,
                             OSVR_IN_PTR OSVR_TimeValue const *timestamp) {
    auto guard = dev->getSendGuard();
    if (guard->lock()) {
        OSVR_DEV_VERBOSE("Sending image report " << metadata.width << "x"
            << metadata.height);
        iface->imaging->sendImageData(metadata, imageData, sensor, *timestamp);
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_FAILURE;
}