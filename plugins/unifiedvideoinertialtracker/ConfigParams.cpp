/** @file
    @brief Implementation

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "ConfigParams.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    ConfigParams::ConfigParams() {
        const double positionNoise = 0.1661102065530816;
        const double rotationNoise = 0.07814149202850287;
        processNoiseAutocorrelation[0] = positionNoise;
        processNoiseAutocorrelation[1] = positionNoise;
        processNoiseAutocorrelation[2] = positionNoise;
        processNoiseAutocorrelation[3] = rotationNoise;
        processNoiseAutocorrelation[4] = rotationNoise;
        processNoiseAutocorrelation[5] = rotationNoise;

        /// If you use manual beacon offset (aka turn off offsetToCentroid),
        /// this is a good default since it's the best beacon offset for the
        /// HDK we've found so far - centroid of front beacons, with only z
        /// component retained.
        manualBeaconOffset[0] = 0;
        manualBeaconOffset[1] = 0;
        manualBeaconOffset[2] = -0.0388676;

        cameraPosition[0] = 0;
        /// @todo this is just a guess of how high my camera is.
        cameraPosition[1] = 1.2;
        /// @todo this is just an estimate of how to not break most apps.
        cameraPosition[2] = -0.5;
    }

    TuningParams::TuningParams() : noveltyPenaltyBase(1.282636090487287) {}
} // namespace vbtracker
} // namespace osvr
