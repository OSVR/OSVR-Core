/** @file
    @brief "One-Euro" tracking filter analysis plugin

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include "EigenFilters.h"

// Generated JSON header file
#include "org_osvr_filter_oneeuro_json.h"

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto DRIVER_NAME = "OneEuroFilter";

using filters::one_euro::Params;
/// Default parameters for a position one-euro filter in meters.
inline Params getPositionDefaults() { return Params{1.15, 0.5, 1.2}; }
/// Default parameters for an orientation one-euro filter using quaternions.
inline Params getOrientationDefaults() { return Params{1.5, 0.5, 1.2}; }

class OneEuroFilterDevice {
  public:
    OneEuroFilterDevice(OSVR_PluginRegContext ctx, std::string const &name,
                        std::string const &input, Params const &posParams,
                        Params const &oriParams)
        : m_posParams(posParams), m_oriParams(oriParams) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_trackerOut);
        /// Create the device token with the options
        m_dev.initSync(ctx, name, opts);
        OSVR_DeviceToken dev;
        if (OSVR_RETURN_FAILURE ==
            osvrAnalysisSyncInit(ctx, name.c_str(), opts, &dev, &m_clientCtx)) {
            throw std::runtime_error("Could not initialize analysis plugin!");
        }
        m_dev = osvr::pluginkit::DeviceToken(dev);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_filter_oneeuro_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        // Nothing to do here - everything happens in a callback.
        return OSVR_RETURN_SUCCESS;
    }

  private:
    const Params m_posParams;
    const Params m_oriParams;

    OSVR_TrackerDeviceInterface m_trackerOut;
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;
};

class AnalysisPluginInstantiation {
  public:
    AnalysisPluginInstantiation() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << "Couldn't parse JSON for one euro filter!"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        // Get filter parameters for position
        auto posParams = getPositionDefaults();
        if (root.isMember("position")) {
            parseOneEuroParams(posParams, root["position"]);
        }

        // Get filter parameters for orientation
        auto oriParams = getOrientationDefaults();
        if (root.isMember("orientation")) {
            parseOneEuroParams(oriParams, root["orientation"]);
        }

        auto input = root["input"].asString();

        auto deviceName = root.get("name", DRIVER_NAME).asString();

        return OSVR_RETURN_SUCCESS;
    }

    static void parseOneEuroParams(Params &p, Json::Value const &json) {
        // In all cases, using the existing value as default value.
        p.minCutoff = json.get("minCutoff", p.minCutoff).asDouble();
        p.beta = json.get("beta", p.beta).asDouble();
        p.derivativeCutoff =
            json.get("derivativeCutoff", p.derivativeCutoff).asDouble();
    }
};
} // namespace

OSVR_PLUGIN(org_osvr_filter_oneeuro) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(DRIVER_NAME,
        new AnalysisPluginInstantiation());

    return OSVR_RETURN_SUCCESS;
}
