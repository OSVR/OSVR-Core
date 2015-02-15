/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "DevicesWithParameters.h"
#include "GetSerialPortState.h"
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

#include <vrpn_YEI_3Space.h>

// Standard includes
#include <vector>
#include <memory>
#include <string>
#include <string.h>

namespace {
/// @brief Manage an array of dynamically allocated c-strings, terminated with a
/// null entry.
class CStringArray : boost::noncopyable {
  public:
    typedef std::unique_ptr<char[]> UniqueCharArray;
    void push_back(std::string const &str) {
        // Remove null terminator from array
        if (m_arrayHasNullTerminator()) {
            m_data.pop_back();
        }
        {
            const size_t stringLength = str.size() + 1;
            UniqueCharArray copy(new char[stringLength]);
            memcpy(copy.get(), str.c_str(), stringLength);
            m_dataOwnership.push_back(std::move(copy));
        }
        m_data.push_back(m_dataOwnership.back().get());
    }
    const char **get_array() {
        // Ensure null terminator on array
        if (!m_arrayHasNullTerminator()) {
            m_data.push_back(nullptr);
        }
        return m_data.data();
    }

  private:
    bool m_arrayHasNullTerminator() const {
        return !m_data.empty() && nullptr == m_data.back();
    }
    std::vector<const char *> m_data;
    std::vector<UniqueCharArray> m_dataOwnership;
};
} // namespace

void createYEI(VRPNMultiserverData &data, OSVR_PluginRegContext ctx,
               const char *params) {
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(params, root)) {
        throw std::runtime_error("Could not parse configuration: " +
                                 reader.getFormattedErrorMessages());
    }
    std::string port = normalizeAndVerifySerialPort(root["port"].asString());
    bool calibrate_gyros_on_setup =
        root.get("calibrateGyrosOnSetup", false).asBool();
    bool tare_on_setup = root.get("tareOnSetup", false).asBool();
    double frames_per_second = root.get("framesPerSecond", 250).asFloat();

    Json::Value commands = root.get("resetCommands", Json::arrayValue);
    CStringArray reset_commands;

    for (Json::ArrayIndex i = 0, e = commands.size(); i < e; ++i) {
        reset_commands.push_back(commands[i].asString());
    }

    osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);

    reg.registerDevice(new vrpn_YEI_3Space_Sensor(
        reg.useDecoratedName(data.getName("YEI_3Space_Sensor")).c_str(),
        reg.getVRPNConnection(), port.c_str(), 115200, calibrate_gyros_on_setup,
        tare_on_setup, frames_per_second, 0, 0, 1, 0,
        reset_commands.get_array()));
}