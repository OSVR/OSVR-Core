/** @file
@brief Implementation for class that tracks and identifies LEDs.

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

#include "BeaconBasedPoseEstimator.h"

namespace osvr {
namespace vbtracker {

// Default 3D locations for the beacons on an OSVR HDK, in millimeters
const std::vector< std::vector<double> > OsvrHdkLedLocations_DEFAULT =
{     { 1, 23.8, 0 }
    , { 11, 5.8, 0 }
    , { 9, 23.8, 0 }
    , { 0, 8.8, 0 }
    , { 9, 23.8, 0 }
    , { 12, 5.8, 0 }
    , { 85.0, 3.00, 24.09 }
    , { 83.2, 14.01, 13.89 }
    , { 47.0, 51.0, 24.09 }
    , { 47.00, 51.0, 24.09 }
    , { 86.6, 2.65, 24.09 }
    , { 85.5, 14.31, 13.89 }
    , { 85.2, 19.68, 13.89 }
    , { 21.00, 51.0, 13.09 }
    , { 21.00, 51.0, 13.09 }
    , { 84.2, 19.99, 13.89 }
    , { 60.41, 47.55, 44.6 }
    , { 80.42, 20.48, 42.9 }
    , { 82.01, 2.74, 42.4 }
    , { 80.42, 14.99, 42.9 }
    , { 60.41, 10.25, 48.1 }
    , { 60.41, 15.75, 48.1 }
    , { 30.41, 32.75, 50.5 }
    , { 31.41, 47.34, 47 }
    , { 0.41, 15.25, 51.3 }
    , { 30.41, 27.25, 50.5 }
    , { 60.44, 45.1, 41.65 }
    , { 22.41, 41.65, 47.8 }
    , { 21.59, 41.65, 47.8 }
    , { 59.59, 41.65, 45.1 }
    , { 79.63, 14.98, 42.9 }
    , { 29.59, 27.25, 50.5 }
    , { 81.19, 2.74, 42.4 }
    , { 79.61, 20.48, 42.9 }
    , { 59.59, 47.55, 44.6 }
    , { 30.59, 47.55, 47 }
    , { 29.59, 32.75, 50.5 }
    , { 0.41, 20.75, 51.3 }
    , { 59.59, 15.75, 48.1 }
    , { 59.59, 10.25, 48.1 }
};

BeaconBasedPoseEstimator::BeaconBasedPoseEstimator(
    const std::vector < std::vector<double> > &cameraMatrix
    , const std::vector<double> &distCoeffs
    , const std::vector< std::vector<double> > &beacons)
{
    m_beacons = beacons;
    m_cameraMatrix = cameraMatrix;
    m_distCoeffs = distCoeffs;
}

void BeaconBasedPoseEstimator::setBeacons(const std::vector< std::vector<double> > &beacons)
{
    m_beacons = beacons;
}
void BeaconBasedPoseEstimator::setCameraMatrix(const std::vector< std::vector<double> > &cameraMatrix)
{
    m_cameraMatrix = cameraMatrix;
}
void BeaconBasedPoseEstimator::setDistCoeffs(const std::vector<double> &distCoeffs)
{
    m_distCoeffs = distCoeffs;
}


} // End namespace vbtracker
} // End namespace osvr
