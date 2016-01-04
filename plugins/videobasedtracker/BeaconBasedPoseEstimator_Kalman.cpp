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
    static const auto BRIGHT_PENALTY = 8.;
    bool BeaconBasedPoseEstimator::m_kalmanAutocalibEstimator(LedGroup &leds,
                                                              double dt) {
        auto const beaconsSize = m_beacons.size();
        // Default measurement variance (for now, factor) per axis.
        double varianceFactor = 1;

        auto maxBoxRatio = m_params.boundingBoxFilterRatio;
        auto minBoxRatio = 1.f / m_params.boundingBoxFilterRatio;

        auto inBoundsID = std::size_t{0};
        // Default to using all the measurements we can
        auto skipBright = false;
        {
            auto totalLeds = leds.size();
            auto identified = std::size_t{0};
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

                if (led.getMeasurement().knowBoundingBox) {
                    auto boundingBoxRatio =
                        led.getMeasurement().boundingBox.height /
                        led.getMeasurement().boundingBox.width;
                    if (boundingBoxRatio > minBoxRatio &&
                        boundingBoxRatio < maxBoxRatio) {
                        inBoundsRound++;
                    }
                }
            }

/// If we only see a few beacons, they may be as likely to send us spinning as
/// help us keep tracking.
#if 0
            // Now we decide if we want to cut the variance artificially to
            // reduce latency in low-beacon situations
            if (inBoundsID < LOW_BEACON_CUTOFF) {
                varianceFactor = 0.5;
            }
#endif

            if (inBoundsID - inBoundsBright >
                    DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS &&
                m_params.shouldSkipBrightLeds) {
                skipBright = true;
            }
            if (0 == inBoundsID) {
                m_framesWithoutIdentifiedBlobs++;
            } else {
                m_framesWithoutIdentifiedBlobs = 0;
            }
        }

        CameraModel cam;
        cam.focalLength = m_camParams.focalLength();
        cam.principalPoint = cvToVector(m_camParams.principalPoint());
        ImagePointMeasurement meas{cam};

        kalman::ConstantProcess<kalman::PureVectorState<>> beaconProcess;

        const auto maxSquaredResidual =
            m_params.maxResidual * m_params.maxResidual;
        const auto maxZComponent = m_params.maxZComponent;
        kalman::predict(m_state, m_model, dt);

        /// @todo should we be recalculating this for each beacon after each
        /// correction step? The order we filter them in is rather arbitrary...
        Eigen::Matrix3d rotate =
            Eigen::Matrix3d(m_state.getCombinedQuaternion());
        auto numBad = std::size_t{0};
        auto numGood = std::size_t{0};
        for (auto &led : leds) {
            if (!led.identified()) {
                continue;
            }
            auto id = led.getID();
            if (id >= beaconsSize) {
                continue;
            }

            auto &debug = m_beaconDebugData[id];
            debug.seen = true;
            debug.measurement = led.getLocation();
            if (skipBright && led.isBright()) {
                continue;
            }

            // Angle of emission checking
            // If we transform the body-local emission vector, an LED pointed
            // right at the camera will be -Z. Anything with a 0 or positive z
            // component is clearly out, and realistically, anything with a z
            // component over -0.5 is probably fairly oblique. We don't want to
            // use such beacons since they can easily introduce substantial
            // error.
            double zComponent =
                (rotate * cvToVector(m_beaconEmissionDirection[id])).z();
            if (zComponent > 0.) {
                if (m_params.extraVerbose) {
                    std::cout << "Rejecting an LED at " << led.getLocation()
                              << " claiming ID " << led.getOneBasedID()
                              << std::endl;
                }
                /// This means the LED is pointed away from us - so we shouldn't
                /// be able to see it.
                led.markMisidentified();

                /// @todo This could be a mis-identification, or it could mean
                /// we're in a totally messed up state. Do we count this against
                /// ourselves?
                numBad++;
                continue;
            } else if (zComponent > maxZComponent) {
                /// LED is too askew of the camera to provide reliable data, so
                /// skip it.
                continue;
            }

#if 0
            if (led.getMeasurement().knowBoundingBox) {
                /// @todo For right now, if we don't have a bounding box, we're
                /// assuming it's square enough (and only testing for
                /// non-squareness on those who actually do have bounding
                /// boxes). This is very much a temporary situation.
                auto boundingBoxRatio =
                    led.getMeasurement().boundingBox.height /
                    led.getMeasurement().boundingBox.width;
                if (boundingBoxRatio < minBoxRatio ||
                    boundingBoxRatio > maxBoxRatio) {
                    /// skip non-circular blobs.
                    numBad++;
                    continue;
                }
            }
#endif

            auto localVarianceFactor = varianceFactor;
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
            led.markAsUsed();
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
                (led.isBright() ? BRIGHT_PENALTY : 1.) *
                m_beaconMeasurementVariance[id] / led.getMeasurement().area;
            debug.variance = effectiveVariance;
            meas.setVariance(effectiveVariance);

            /// Now, do the correction.
            auto model =
                kalman::makeAugmentedProcessModel(m_model, beaconProcess);
            kalman::correct(state, model, meas);
            m_gotMeasurement = true;
        }

        /// Probation: Dealing with ratios of bad to good residuals
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

        /// Frames without measurements: dealing with getting in a bad state
        if (m_gotMeasurement) {
            m_framesWithoutUtilizedMeasurements = 0;
        } else {
            if (inBoundsID > 0) {
                /// We had a measurement, we rejected it. The problem may be the
                /// plank in our own eye, not the speck in our beacon's eye.
                m_framesWithoutUtilizedMeasurements++;
            }
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
