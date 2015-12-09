/** @file
    @brief Sensor fusion analysis plugin to combine video-based tracking with
   IMU orientation.

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
#include "VideoIMUFusionDevice.h"
#include "FusionParams.h"
#include <osvr/PluginKit/PluginKit.h>

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto DRIVER_NAME = "VideoIMUFusion";

class AnalysisPluginInstantiation {
  public:
    AnalysisPluginInstantiation() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << "Couldn't parse JSON for " << DRIVER_NAME
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        // required: the input paths.
        auto &input = root["input"];
        auto imu = input["imu"].asString();
        auto faceplate = input["faceplate"].asString();

        // optional
        auto deviceName = root.get("name", DRIVER_NAME).asString();

        VideoIMUFusionParams fusionParams;

        if (root.isMember("videoTrackerVariance")) {
            auto &variance = root["videoTrackerVariance"];
            fusionParams.videoPosVariance =
                variance.get("position", fusionParams.videoPosVariance)
                    .asDouble();
            fusionParams.videoOriVariance =
                variance.get("orientation", fusionParams.videoOriVariance)
                    .asDouble();
        }

        if (root.isMember("imuVariance")) {
            auto &variance = root["imuVariance"];
            fusionParams.imuOriVariance =
                variance.get("orientation", fusionParams.imuOriVariance)
                    .asDouble();
            fusionParams.imuAngVelVariance =
                variance.get("angularVelocity", fusionParams.imuAngVelVariance)
                    .asDouble();
        }

        if (root.isMember("processNoise")) {
            auto &noise = root["processNoise"];
            fusionParams.positionNoise =
                noise.get("position", fusionParams.positionNoise).asDouble();
            fusionParams.oriNoise =
                noise.get("orientation", fusionParams.oriNoise).asDouble();
        }

        fusionParams.damping =
            root.get("damping", fusionParams.damping).asDouble();
        fusionParams.eyeHeight =
            root.get("eyeHeight", fusionParams.eyeHeight).asDouble();

        osvr::pluginkit::PluginContext context(ctx);

        /// @todo make the token own this instead once there is API for that.
        context.registerObjectForDeletion(new VideoIMUFusionDevice(
            ctx, deviceName, imu, faceplate, fusionParams));
        return OSVR_RETURN_SUCCESS;
    }
};
} // namespace

OSVR_PLUGIN(org_osvr_filter_videoimufusion) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(DRIVER_NAME,
                                                AnalysisPluginInstantiation());

    return OSVR_RETURN_SUCCESS;
}
