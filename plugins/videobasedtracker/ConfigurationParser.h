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
#include "GetOptionalParameter.h"
#include "Types.h"
#include <ParseBlobParams.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

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
        getOptionalParameter(config.calibrationFile, root, "calibrationFile");
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
        getOptionalParameter(config.streamBeaconDebugInfo, root,
                             "streamBeaconDebugInfo");
        getOptionalParameter(config.offsetToCentroid, root, "offsetToCentroid");
        if (!config.offsetToCentroid) {
            getOptionalParameter(config.manualBeaconOffset, root,
                                 "manualBeaconOffset");
        }

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
        getOptionalParameter(config.boundingBoxFilterRatio, root,
                             "boundingBoxFilterRatio");
        getOptionalParameter(config.maxZComponent, root, "maxZComponent");
        getOptionalParameter(config.shouldSkipBrightLeds, root,
                             "shouldSkipBrightLeds");

        /// Blob-detection parameters
        if (root.isMember("blobParams")) {
            parseBlobParams(root["blobParams"], config.blobParams);
        }

        return config;
    }

} // End namespace vbtracker
} // End namespace osvr
#endif // INCLUDED_ConfigurationParser_h_GUID_933C79EE_3392_4C8D_74D5_D9A72580DA6A
