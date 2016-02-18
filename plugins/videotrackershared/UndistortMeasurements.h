/** @file
    @brief Header

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

#ifndef INCLUDED_UndistortMeasurements_h_GUID_EBA50677_306F_4985_BC70_393C58140FE4
#define INCLUDED_UndistortMeasurements_h_GUID_EBA50677_306F_4985_BC70_393C58140FE4

// Internal Includes
#include "LedMeasurement.h"
#include "cvToEigen.h"
#include "CameraParameters.h"
#include "CameraDistortionModel.h"

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
#include <vector>
#include <algorithm>

namespace osvr {
namespace vbtracker {
    /// Perform the undistortion of LED measurements.
    inline LedMeasurementVec
    undistortLeds(LedMeasurementVec const &distortedMeasurements,
                  CameraParameters const &camParams) {
        LedMeasurementVec ret;
        ret.resize(distortedMeasurements.size());
        auto distortionModel = CameraDistortionModel{
            Eigen::Vector2d{camParams.focalLengthX(), camParams.focalLengthY()},
            cvToVector(camParams.principalPoint()),
            Eigen::Vector3d{camParams.k1(), camParams.k2(), camParams.k3()}};
        auto ledUndistort = [&distortionModel](LedMeasurement const &meas) {
            LedMeasurement ret{meas};
            Eigen::Vector2d undistorted = distortionModel.undistortPoint(
                cvToVector(meas.loc).cast<double>());
            ret.loc = vecToPoint(undistorted.cast<float>());
            return ret;
        };
        std::transform(begin(distortedMeasurements), end(distortedMeasurements),
                       begin(ret), ledUndistort);
        return ret;
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_UndistortMeasurements_h_GUID_EBA50677_306F_4985_BC70_393C58140FE4
