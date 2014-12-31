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
#include "DevicesWithParameters.h"
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

#include <vrpn_YEI_3Space.h>

// Standard includes
// - none

void createYEI(VRPNMultiserverData &data, OSVR_PluginRegContext ctx,
               const char *params) {
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(params, root)) {
        throw std::runtime_error("Could not parse configuration: " +
                                 reader.getFormattedErrorMessages());
    }
    std::string port = root["port"].asString();
    bool calibrate_gyros_on_setup =
        root.get("calibrateGyrosOnSetup", false).asBool();
    bool tare_on_setup = root.get("tareOnSetup", false).asBool();
    double frames_per_second = root.get("framesPerSecond", 250).asFloat();

    osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);

    reg.registerDevice(new vrpn_YEI_3Space_Sensor(
        reg.useDecoratedName(data.getName("YEI_3Space_Sensor")).c_str(),
        reg.getVRPNConnection(), port.c_str(), 115200, calibrate_gyros_on_setup,
        tare_on_setup, frames_per_second, 0, 0, 1, 0));
}