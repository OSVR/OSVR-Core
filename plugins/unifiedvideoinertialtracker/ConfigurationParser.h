/** @file
    @brief Header

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ConfigurationParser_h_GUID_933C79EE_3392_4C8D_74D5_D9A72580DA6A
#define INCLUDED_ConfigurationParser_h_GUID_933C79EE_3392_4C8D_74D5_D9A72580DA6A

// Internal Includes
#include "Types.h"
#include "GetOptionalParameter.h"
#include "OptionalStream.h"

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    static const auto MESSAGE_PREFIX =
        "[Unified Tracker] Configuration Parsing WARNING: ";
#define PARAMNAME(X) "'" << X << "'"
    inline ConfigParams parseConfigParams(Json::Value const &root) {
        ConfigParams config;
        config.debug = root.get("showDebug", false).asBool();
        /// Rear panel stuff
        getOptionalParameter(config.includeRearPanel, root, "includeRearPanel");
        getOptionalParameter(config.headCircumference, root,
                             "headCircumference");
        getOptionalParameter(config.headToFrontBeaconOriginDistance, root,
                             "headToFrontBeaconOriginDistance");
        getOptionalParameter(config.backPanelMeasurementError, root,
                             "backPanelMeasurementError");

        // If we include the rear panel, we default to not offsetting to
        // centroid since it causes strange tracking.
        if (config.includeRearPanel) {
            config.offsetToCentroid = false;
        }

        /// General parameters
        getOptionalParameter(config.extraVerbose, root, "extraVerbose");
#if 0
        getOptionalParameter(config.calibrationFile, root, "calibrationFile");
#else
        outputUnless(std::cout, root["calibrationFile"].isNull())
            << MESSAGE_PREFIX << PARAMNAME("calibrationFile")
            << " not yet implemented in the new tracker";
#endif

        getOptionalParameter(config.additionalPrediction, root,
                             "additionalPrediction");
        getOptionalParameter(config.maxResidual, root, "maxResidual");
        getOptionalParameter(config.initialBeaconError, root,
                             "initialBeaconError");
        getOptionalParameter(config.blobMoveThreshold, root,
                             "blobMoveThreshold");
        getOptionalParameter(config.blobsKeepIdentity, root,
                             "blobsKeepIdentity");
        getOptionalParameter(config.numThreads, root, "numThreads");
#if 0
        getOptionalParameter(config.streamBeaconDebugInfo, root,
                             "streamBeaconDebugInfo");
#else
        outputUnless(std::cout, root["streamBeaconDebugInfo"].isNull())
            << MESSAGE_PREFIX << PARAMNAME("streamBeaconDebugInfo")
            << " not yet implemented in the new tracker";
#endif

        getOptionalParameter(config.offsetToCentroid, root, "offsetToCentroid");
        if (!config.offsetToCentroid) {
            getOptionalParameter(config.manualBeaconOffset, root,
                                 "manualBeaconOffset");
        }

        /// Fusion/Calibration parameters
        getOptionalParameter(config.cameraPosition, root, "cameraPosition");
        getOptionalParameter(config.cameraIsForward, root, "cameraIsForward");
        outputUnless(std::cout, root["eyeHeight"].isNull())
            << MESSAGE_PREFIX << PARAMNAME("eyeHeight")
            << " is deprecated/ignored: use 'cameraPosition' for similar "
               "effects with this plugin.";

        /// Kalman-related parameters
        getOptionalParameter(config.beaconProcessNoise, root,
                             "beaconProcessNoise");
        getOptionalParameter(config.processNoiseAutocorrelation, root,
                             "processNoiseAutocorrelation");
        getOptionalParameter(config.linearVelocityDecayCoefficient, root,
                             "linearVelocityDecayCoefficient");
        getOptionalParameter(config.angularVelocityDecayCoefficient, root,
                             "angularVelocityDecayCoefficient");
        getOptionalParameter(config.measurementVarianceScaleFactor, root,
                             "measurementVarianceScaleFactor");
        getOptionalParameter(config.highResidualVariancePenalty, root,
                             "highResidualVariancePenalty");
#if 0
        getOptionalParameter(config.boundingBoxFilterRatio, root,
                             "boundingBoxFilterRatio");
#else
        outputUnless(std::cout, root["boundingBoxFilterRatio"].isNull())
            << MESSAGE_PREFIX << PARAMNAME("boundingBoxFilterRatio")
            << " parameter not actively used";
#endif
        getOptionalParameter(config.maxZComponent, root, "maxZComponent");
        getOptionalParameter(config.shouldSkipBrightLeds, root,
                             "shouldSkipBrightLeds");
        getOptionalParameter(config.brightLedVariancePenalty, root,
                             "brightLedVariancePenalty");

        /// Blob-detection parameters
        if (root.isMember("blobParams")) {
            Json::Value const &blob = root["blobParams"];

            getOptionalParameter(config.blobParams.absoluteMinThreshold, blob,
                                 "absoluteMinThreshold");
            getOptionalParameter(config.blobParams.minDistBetweenBlobs, blob,
                                 "minDistBetweenBlobs");
            getOptionalParameter(config.blobParams.minArea, blob, "minArea");
            getOptionalParameter(config.blobParams.filterByCircularity, blob,
                                 "filterByCircularity");
            getOptionalParameter(config.blobParams.minCircularity, blob,
                                 "minCircularity");
            getOptionalParameter(config.blobParams.filterByConvexity, blob,
                                 "filterByConvexity");
            getOptionalParameter(config.blobParams.minConvexity, blob,
                                 "minConvexity");
            getOptionalParameter(config.blobParams.minThresholdAlpha, blob,
                                 "minThresholdAlpha");
            getOptionalParameter(config.blobParams.maxThresholdAlpha, blob,
                                 "maxThresholdAlpha");
            getOptionalParameter(config.blobParams.thresholdSteps, blob,
                                 "thresholdSteps");
        }

        /// IMU-related parameters
        if (root.isMember("imu")) {
            Json::Value const &imu = root["imu"];
            getOptionalParameter(config.imu.path, imu, "path");
            getOptionalParameter(config.imu.useOrientation, imu,
                                 "useOrientation");
            getOptionalParameter(config.imu.orientationVariance, imu,
                                 "orientationVariance");
            getOptionalParameter(config.imu.useAngularVelocity, imu,
                                 "useAngularVelocity");
            getOptionalParameter(config.imu.angularVelocityVariance, imu,
                                 "angularVelocityVariance");
        }

        return config;
    }
#undef PARAMNAME
} // End namespace vbtracker
} // End namespace osvr
#endif // INCLUDED_ConfigurationParser_h_GUID_933C79EE_3392_4C8D_74D5_D9A72580DA6A
