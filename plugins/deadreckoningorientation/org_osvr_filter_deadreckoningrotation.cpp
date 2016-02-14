/** @file
    @brief Analysis plugin that performs predictive tracking for the orientation
   if a specified tracker and is provided a prediction interval.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>
#include <osvr/Util/StringLiteralFileToString.h>
#include <vrpn_Tracker_Filter.h>

// Generated JSON header file
#include "org_osvr_filter_deadreckoningrotation_json.h"

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <chrono>
#include <thread>

// Anonymous namespace to avoid symbol collision
namespace {

class DeadReckoningRotation {
  public:
    DeadReckoningRotation(OSVR_PluginRegContext ctx, std::string const &name,
      std::string const &input, int numSensors, double predictMS)
    {
        /// Register the VRPN device that we will be using.
        osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);
        std::string decoratedName = reg.useDecoratedName(name);
        std::string localInput = "*" + input;
        reg.registerDevice(new vrpn_Tracker_DeadReckoning_Rotation(
          decoratedName,
          reg.getVRPNConnection(),
          localInput, numSensors, predictMS));
        reg.setDeviceDescriptor(osvr::util::makeString(
          org_osvr_filter_deadreckoningrotation_json));

        // Build a new Json entry that has the correct number of
        // sensors in it, rather than the default of 1.
        {
          Json::Reader reader;
          Json::Value filterJson;
          if (!reader.parse(
            osvr::util::makeString(
            org_osvr_filter_deadreckoningrotation_json),
            filterJson)) {
            throw std::logic_error("Faulty JSON file for Dead "
              "Reckoning Rotation Filter - should not "
              "be possible!");
          }
          filterJson["interfaces"]["tracker"]["count"] = numSensors;

          // Corresponding filter
          reg.setDeviceDescriptor(filterJson.toStyledString());
        }
    }

    OSVR_ReturnCode update() {
      // Nothing to do here - everything happens in a callback.
      return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_trackerOut;
};

class DeadReckoningRotationConstructor {
  public:
    /// @brief This is the required signature for a device instantiation
    /// callback.
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        // Read the JSON data from parameters.
        Json::Value root;
        if (params) {
            Json::Reader r;
            if (!r.parse(params, root)) {
                std::cerr << "Could not parse parameters!" << std::endl;
            }
        }

        // Get the name we should use for the device
        if (!root.isMember("name")) {
            std::cerr << "Error: got configuration, but no name specified."
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }
        std::string name = root["name"].asString();

        // Get the input device we should use to listen to
        if (!root.isMember("input")) {
          std::cerr << "Error: got configuration, but no input specified."
            << std::endl;
          return OSVR_RETURN_FAILURE;
        }
        std::string input = root["input"].asString();

        int numSensors = root.get("numSensors", 1).asInt();

        double predictMS = root.get("predictMilliSeconds", 32).asDouble();

        // OK, now that we have our parameters, create the device.
        osvr::pluginkit::registerObjectForDeletion(
          ctx, new DeadReckoningRotation(ctx, name, input, numSensors, predictMS * 1e-3));

        return OSVR_RETURN_SUCCESS;
    }
};
} // namespace

OSVR_PLUGIN(com_osvr_example_Configured) {

    /// Tell the core we're available to create a device object.
    osvr::pluginkit::registerDriverInstantiationCallback(
      ctx, "DeadReckoningRotationTracker", new DeadReckoningRotationConstructor);

    return OSVR_RETURN_SUCCESS;
}
