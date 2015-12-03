/** @file
    @brief Implementation of Kalman-specific code in beacon-based pose
   estimator, to reduce incremental build times.

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

#include <iostream>
#include <osvr/Util/EigenCoreGeometry.h>
#if 0
template <typename T>
inline void dumpKalmanDebugOuput(const char name[], const char expr[],
                                 T const &value) {
    std::cout << "\n(Kalman Debug Output) " << name << " [" << expr << "]:\n"
              << value << std::endl;
}
#endif

// Internal Includes
#include "BeaconBasedPoseEstimator.h"
#include "VideoJacobian.h"
#include "ImagePointMeasurement.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/AugmentedProcessModel.h>
#include <osvr/Kalman/AugmentedState.h>
#include <osvr/Kalman/ConstantProcess.h>

#include <osvr/Util/EigenInterop.h>

#include <opencv2/core/eigen.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// This is the constant maximum distance in image space (pixels) permitted
    /// between a projected beacon location and its detected location.
    static const auto MAX_RESIDUAL = 75.0;
    static const auto MAX_SQUARED_RESIDUAL = MAX_RESIDUAL * MAX_RESIDUAL;

    bool
    BeaconBasedPoseEstimator::m_kalmanAutocalibEstimator(const LedGroup &leds,
                                                         double dt) {
        auto const beaconsSize = m_beacons.size();
        CameraModel cam;
        cam.focalLength = m_focalLength;
        cam.principalPoint = m_principalPoint;
        ImagePointMeasurement meas{cam};
        kalman::ConstantProcess<kalman::PureVectorState<>> beaconProcess;
        Eigen::Vector2d pt;

        kalman::predict(m_state, m_model, dt);

        auto numBad = std::size_t{0};
        auto numGood = std::size_t{0};
        for (auto const &led : leds) {
            if (!led.identified()) {
                continue;
            }
            auto id = led.getID();
            if (id >= beaconsSize) {
                continue;
            }
            meas.setMeasurement(
                Eigen::Vector2d(led.getLocation().x, led.getLocation().y));

            auto state = kalman::makeAugmentedState(m_state, *(m_beacons[id]));
            meas.updateFromState(state);
            auto model =
                kalman::makeAugmentedProcessModel(m_model, beaconProcess);

            if (meas.getResidual(state).squaredNorm() > MAX_SQUARED_RESIDUAL) {
                // probably bad
                std::cout << "skipping a measurement with a high residual: id "
                          << id << std::endl;
                numBad++;
                continue;
            }
            numGood++;

            kalman::correct(state, model, meas);
        }

        bool incrementProbation = false;
        if (0 == m_framesInProbation) {
            // Let's try to keep a 3:2 ratio of good to bad when not "in
            // probation"
            incrementProbation = (numBad * 3 > numGood * 2);

        } else {
            // Already in trouble, add a bit of hysteresis and raising the bar
            // so we don't hop out easily.
            incrementProbation = numBad * 2 > numGood;
            if (!incrementProbation) {
                // OK, we're good again
                std::cout << "Re-attained our tracking goal." << std::endl;
                m_framesInProbation = 0;
            }
        }
        if (incrementProbation) {
            std::cout << "Fell below our target for tracking residuals: "
                      << numBad << " bad, " << numGood << " good." << std::endl;
            m_framesInProbation++;
        }
        /// Output to the OpenCV state types so we can see the reprojection
        /// debug view.
        m_rvec = eiQuatToRotVec(m_state.getQuaternion());
        cv::eigen2cv(m_state.getPosition().eval(), m_tvec);
        return true;
    }

    /// millimeters to meters
    static const double LINEAR_SCALE_FACTOR = 1000.;
    OSVR_PoseState
    BeaconBasedPoseEstimator::GetPredictedState(double dt) const {
        auto state = m_state;
        auto model = m_model;
        kalman::predict(state, model, dt);
        state.postCorrect();
        OSVR_PoseState ret;
        util::eigen_interop::map(ret).rotation() = state.getQuaternion();
        util::eigen_interop::map(ret).translation() =
            state.getPosition() / LINEAR_SCALE_FACTOR; // convert from mm to m
        return ret;
    }

} // namespace vbtracker
} // namespace osvr
