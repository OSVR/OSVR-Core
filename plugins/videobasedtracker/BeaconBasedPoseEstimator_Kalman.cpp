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

#include <opencv2/core/eigen.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

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

        static int i = 0;
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
            auto model =
                kalman::makeAugmentedProcessModel(m_model, beaconProcess);
            if (meas.getResidual(state).squaredNorm() > 2500) {
                // more than 50 pixels off - probably bad
                std::cout << "skipping a measurement with a high residual"
                          << std::endl;
                continue;
            }
            if (i == 0) {
                std::cout << "beacon " << id << " residual "
                          << meas.getResidual(state).transpose() << "\n";
            }
            kalman::correct(state, model, meas);
        }
        i = (i + 1) % 200;

        /// Output to the OpenCV state types so we can see the reprojection
        /// debug view.
        m_rvec = eiQuatToRotVec(m_state.getQuaternion());
        cv::eigen2cv(m_state.getPosition().eval(), m_tvec);
        return true;
    }

    static const double InitialStateError[] = {
        10., 10., 10., 1., 1., 1., 100., 100., 100., 100., 100., 100.};
    void
    BeaconBasedPoseEstimator::m_resetState(Eigen::Vector3d const &xlate,
                                           Eigen::Quaterniond const &quat) {
        using StateVec = kalman::types::DimVector<State>;
        StateVec state(StateVec::Zero());
        state.head<3>() = xlate;
        m_state.setStateVector(state);
        m_state.setQuaternion(quat);
        m_state.setErrorCovariance(StateVec(InitialStateError).asDiagonal());
        std::cout << "State:" << m_state.stateVector().transpose()
                  << "\n  with quaternion "
                  << m_state.getQuaternion().coeffs().transpose() << std::endl;
    }

} // namespace vbtracker
} // namespace osvr
