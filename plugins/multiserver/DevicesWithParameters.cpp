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
#include <vector>
#include <string>
#include <string.h>

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

#ifdef _WIN32
    // Use the Win32 device namespace, if they aren't already.
    // Have to double the backslashes because they're escape characters.
    /// @todo device namespace only valid on WinNT-derived windows?
    if (port.find('\\') == std::string::npos) {
        port = "\\\\.\\" + port;
    }
#endif

    std::vector<std::string> string_reset_commands;
    Json::Value commands = root.get("resetCommands", Json::arrayValue);

    for (Json::ArrayIndex i = 0, e = commands.size(); i < e; ++i) {
        string_reset_commands.push_back(commands[i].asString());
    }

    // Transform the vector of strings into an array of char*, terminated by a
    // null.
    size_t num_reset_commands = string_reset_commands.size() + 1;
    const char **reset_commands = new const char *[num_reset_commands];
    reset_commands[num_reset_commands - 1] = nullptr;
    for (size_t i = 0; i < string_reset_commands.size(); i++) {
        char *command = new char[string_reset_commands[i].size() + 1];
        strcpy(command, string_reset_commands[i].c_str());
        reset_commands[i] = command;
    }

    osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);

    reg.registerDevice(new vrpn_YEI_3Space_Sensor(
        reg.useDecoratedName(data.getName("YEI_3Space_Sensor")).c_str(),
        reg.getVRPNConnection(), port.c_str(), 115200, calibrate_gyros_on_setup,
        tare_on_setup, frames_per_second, 0, 0, 1, 0, reset_commands));

    // Free the command data
    for (size_t i = 0; i < num_reset_commands; i++) {
        delete[] reset_commands[i];
    }
    delete[] reset_commands;
}