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
    static const auto LOW_BEACON_CUTOFF = 5;

    static const auto DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS = 4;
    bool
    BeaconBasedPoseEstimator::m_kalmanAutocalibEstimator(LedGroup &leds,
                                                         double dt) {
        auto const beaconsSize = m_beacons.size();
        // Default measurement variance (for now, factor) per axis.
        double varianceFactor = 1;

        auto maxBoxRatio = m_params.boundingBoxFilterRatio;
        auto minBoxRatio = 1.f / m_params.boundingBoxFilterRatio;

        // Default to using all the measurements we can
        auto skipBright = false;
        {
            auto totalLeds = leds.size();
            auto identified = std::size_t{0};
            auto inBoundsID = std::size_t{0};
            auto inBoundsBright = std::size_t{0};
            auto inBoundsRound = std::size_t{0};
            for (auto const &led : leds) {
                if (!led.identified()) {
                    continue;
                }
                identified++;
                auto id = led.getID();
                if (id >= beaconsSize) {
                    continue;
                }
                inBoundsID++;
                if (led.isBright()) {
                    inBoundsBright++;
                }

                auto boundingBoxRatio =
                    led.getMeasurement().boundingBox.height /
                    led.getMeasurement().boundingBox.width;
                if (boundingBoxRatio > minBoxRatio &&
                    boundingBoxRatio < maxBoxRatio) {
                    inBoundsRound++;
                }
            }

            // Now we decide if we want to cut the variance artificially to
            // reduce latency in low-beacon situations
            if (inBoundsID < LOW_BEACON_CUTOFF) {
                varianceFactor = 0.5;
            }
            if (inBoundsID - inBoundsBright >
                DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS) {
                skipBright = true;
            } else {
#if 0
                if (m_params.debug) {
                    std::cout << "Can't afford to skip brights this frame"
                              << std::endl;
                }
#endif
            }
        }

        CameraModel cam;
        cam.focalLength = m_camParams.focalLength();
        cam.principalPoint = cvToVector(m_camParams.principalPoint());
        ImagePointMeasurement meas{cam};

        kalman::ConstantProcess<kalman::PureVectorState<>> beaconProcess;
        Eigen::Vector2d pt;

        const auto maxSquaredResidual =
            m_params.maxResidual * m_params.maxResidual;

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
            auto boundingBoxRatio = led.getMeasurement().boundingBox.height /
                                    led.getMeasurement().boundingBox.width;
            if (boundingBoxRatio < minBoxRatio ||
                boundingBoxRatio > maxBoxRatio) {
                /// skip non-circular blobs.
                continue;
            }
            auto &debug = m_beaconDebugData[id];
            debug.seen = true;
            debug.measurement = led.getLocation();
            auto localVarianceFactor = varianceFactor;
            if (skipBright && led.isBright()) {
                continue;
            }
            auto newIdentificationVariancePenalty =
                std::pow(2.0, led.novelty());

            /// Stick a little bit of process model uncertainty in the beacon,
            /// if it's meant to have some
            if (m_beaconFixed[id]) {
                beaconProcess.setNoiseAutocorrelation(0);
            } else {
                beaconProcess.setNoiseAutocorrelation(
                    m_params.beaconProcessNoise);
                kalman::predict(*(m_beacons[id]), beaconProcess, dt);
            }

            meas.setMeasurement(
                Eigen::Vector2d(led.getLocation().x, led.getLocation().y));

            auto state = kalman::makeAugmentedState(m_state, *(m_beacons[id]));
            meas.updateFromState(state);
            Eigen::Vector2d residual = meas.getResidual(state);
            if (residual.squaredNorm() > maxSquaredResidual) {
                // probably bad
                numBad++;
                localVarianceFactor *= m_params.highResidualVariancePenalty;
            } else {
                numGood++;
            }
            debug.residual.x = residual.x();
            debug.residual.y = residual.y();
            auto effectiveVariance =
                localVarianceFactor * m_params.measurementVarianceScaleFactor *
                newIdentificationVariancePenalty *
                m_beaconMeasurementVariance[id] / led.getMeasurement().area;
            debug.variance = effectiveVariance;
            meas.setVariance(effectiveVariance);

            /// Now, do the correction.
            auto model =
                kalman::makeAugmentedProcessModel(m_model, beaconProcess);
            kalman::correct(state, model, meas);
            m_gotMeasurement = true;
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
                m_framesInProbation = 0;
            }
        }
        if (incrementProbation) {
            m_framesInProbation++;
        }
        /// Output to the OpenCV state types so we can see the reprojection
        /// debug view.
        m_rvec = eiQuatToRotVec(m_state.getQuaternion());
        cv::eigen2cv(m_state.position().eval(), m_tvec);
        return true;
    }

    OSVR_PoseState
    BeaconBasedPoseEstimator::GetPredictedState(double dt) const {
        auto state = m_state;
        auto model = m_model;
        kalman::predict(state, model, dt);
        state.postCorrect();
        OSVR_PoseState ret;
        util::eigen_interop::map(ret).rotation() = state.getQuaternion();
        util::eigen_interop::map(ret).translation() =
            m_convertInternalPositionRepToExternal(state.position());
        return ret;
    }

} // namespace vbtracker
} // namespace osvr
