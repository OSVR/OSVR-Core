/** @file
    @brief Implementation

    @date 2015, 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015, 2016 Sensics, Inc.
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

#include <util/Stride.h>
#include <iostream>
#include <osvr/Util/EigenCoreGeometry.h>
static ::util::Stride debugStride{401};

#if 0
template <typename T>
inline void dumpKalmanDebugOuput(const char name[], const char expr[],
                                 T const &value) {
    if (debugStride) {
        std::cout << "\n(Kalman Debug Output) " << name << " [" << expr
                  << "]:\n" << value << std::endl;
    }
}
#define OSVR_KALMAN_DEBUG_OUTPUT(Name, Value)                                  \
    dumpKalmanDebugOuput(Name, #Value, Value)
#endif

// Internal Includes
#include "PoseEstimator_SCAATKalman.h"
#include "ImagePointMeasurement.h"
#include "LED.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/AugmentedProcessModel.h>
#include <osvr/Kalman/AugmentedState.h>
#include <osvr/Kalman/ConstantProcess.h>

#include <util/Stride.h>

// Standard includes
#include <algorithm>
#include <iostream>
#include <random> // std::default_random_engine
#include <chrono> // std::chrono::system_clock
#include <iterator> // back_inserter

#undef DEBUG_MEASUREMENT_RESIDUALS

namespace osvr {
namespace vbtracker {
    static const auto DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS = 4;
    SCAATKalmanPoseEstimator::SCAATKalmanPoseEstimator(
        ConfigParams const &params)
        : m_shouldSkipBright(params.shouldSkipBrightLeds),
          m_maxSquaredResidual(params.maxResidual * params.maxResidual),
          m_maxZComponent(params.maxZComponent),
          m_highResidualVariancePenalty(params.highResidualVariancePenalty),
          m_beaconProcessNoise(params.beaconProcessNoise),
          m_brightLedVariancePenalty(params.brightLedVariancePenalty),
          m_measurementVarianceScaleFactor(
              params.measurementVarianceScaleFactor),
          m_extraVerbose(params.extraVerbose),
          m_randEngine(
              std::chrono::system_clock::now().time_since_epoch().count()) {
        std::tie(m_minBoxRatio, m_maxBoxRatio) =
            std::minmax({params.boundingBoxFilterRatio,
                         1.f / params.boundingBoxFilterRatio});

        const auto maxSquaredResidual = params.maxResidual * params.maxResidual;
    }

    inline double xyDistanceFromMetersToPixels(double xyDistance,
                                               double depthInMeters,
                                               CameraModel const &cam) {
        return (xyDistance / depthInMeters) * cam.focalLength;
    }
    inline double squaredXyDistanceFromMetersToPixels(double xyDistance,
                                                      double depthInMeters,
                                                      CameraModel const &cam) {
        return (xyDistance * xyDistance / (depthInMeters * depthInMeters)) *
               cam.focalLength * cam.focalLength;
    }

    bool SCAATKalmanPoseEstimator::
    operator()(EstimatorInOutParams const &p, LedPtrList const &leds,
               osvr::util::time::TimeValue const &frameTime, double videoDt) {
        bool gotMeasurement = false;
        double varianceFactor = 1;

        const auto inBoundsID = leds.size();
        // Default to using all the measurements we can
        auto skipBright = false;

        auto skipAll = false;
        auto numBeaconsToUse = std::size_t{0};
        {
            auto inBoundsBright = std::size_t{0};
            auto inBoundsRound = std::size_t{0};

            /// Count up types of beacons
            for (auto const &ledPtr : leds) {
                auto &led = *ledPtr;
                if (led.isBright()) {
                    inBoundsBright++;
                }

                auto boundingBoxRatioResult = inBoundingBoxRatioRange(led);
                if (TriBool::True == boundingBoxRatioResult) {
                    inBoundsRound++;
                }
            }

            /// Decisions based on the counts.
            if (inBoundsID - inBoundsBright >
                    DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS &&
                m_shouldSkipBright) {
                skipBright = true;
            }
            if (0 == inBoundsID) {
                m_framesWithoutIdentifiedBlobs++;
            } else {
                m_framesWithoutIdentifiedBlobs = 0;
            }
            numBeaconsToUse =
                skipBright ? (inBoundsID - inBoundsBright) : inBoundsID;
            if (m_lastUsableBeaconsSeen > 0 && numBeaconsToUse == 1) {
                // If we're losing sight of beacons and we can only see one,
                // it's likely to give us nasty velocity, so in that case we
                // skip it too.
                skipAll = true;
                // Set this here, because goodLeds will be empty below.
                m_lastUsableBeaconsSeen = numBeaconsToUse;
            }
        }

        if (p.startingTime != frameTime) {
            /// Predict first if appropriate.
            auto dt = util::time::duration(frameTime, p.startingTime);
            // auto dt = osvrTimeValueDurationSeconds(&frameTime,
            // &p.startingTime);
            kalman::predict(p.state, p.processModel, dt);
            p.state.externalizeRotation();
        }

        auto numBad = std::size_t{0};
        auto numGood = std::size_t{0};
        auto goodLeds = filterLeds(leds, skipBright, skipAll, numBad, p);

        if (!skipAll) {
            // if we were in skipAll mode, we set this above.
            m_lastUsableBeaconsSeen = goodLeds.size();
        }

        /// Shuffle the order of the good LEDS
        std::shuffle(begin(goodLeds), end(goodLeds), m_randEngine);

#if 0
        /// Limit the number of measurements filtered in each time.
        static const auto MAX_MEASUREMENTS = 15;
        if (goodLeds.size() > MAX_MEASUREMENTS) {
            goodLeds.resize(MAX_MEASUREMENTS);
        }
#endif

#if 0
        static ::util::Stride varianceStride{ 809 };
        if (++varianceStride) {
            std::cout << p.state.errorCovariance().diagonal().transpose()
                << std::endl;
        }
#endif
#ifdef DEBUG_MEASUREMENT_RESIDUALS
        static ::util::Stride s{203};
        s.advance();
#endif

        CameraModel cam;
        cam.focalLength = p.camParams.focalLength();
        cam.principalPoint = p.camParams.eiPrincipalPoint();
        ImagePointMeasurement meas{cam, p.targetToBody};

        kalman::ConstantProcess<kalman::PureVectorState<>> beaconProcess;

        for (auto &ledPtr : goodLeds) {
            auto &led = *ledPtr;

            auto id = led.getID();
            auto index = asIndex(id);

            auto &debug = p.beaconDebug[index];

            auto localVarianceFactor = varianceFactor;
            auto newIdentificationVariancePenalty =
                std::pow(2.0, led.novelty());

            /// Stick a little bit of process model uncertainty in the beacon,
            /// if it's meant to have some
            if (p.beaconFixed[index]) {
                beaconProcess.setNoiseAutocorrelation(0);
#ifdef VARIANCE_PENALTY_FOR_FIXED_BEACONS
                /// Add a bit of variance to the fixed ones, since the lack of
                /// beacon autocalib otherwise make them seem
                /// super-authoritative.
                localVarianceFactor *= 2;
#endif
            } else {
                beaconProcess.setNoiseAutocorrelation(m_beaconProcessNoise);
                kalman::predict(*(p.beacons[index]), beaconProcess, videoDt);
            }

            /// subtracting from image size to flip signs of x and y, aka 180
            /// degree rotation about z axis.
            meas.setMeasurement(cvToVector(led.getLocationForTracking()).cast<double>());

            led.markAsUsed();
            auto state =
                kalman::makeAugmentedState(p.state, *(p.beacons[index]));
            meas.updateFromState(state);
            Eigen::Vector2d residual = meas.getResidual(state);
            auto depth = meas.getBeaconInCameraSpace().z();
            // Only tolerate a residual of 0.15m at the beacon depth.
            auto maxSquaredResidual =
                squaredXyDistanceFromMetersToPixels(0.15, depth, cam);
            if (residual.squaredNorm() > maxSquaredResidual) {
                // probably bad
                numBad++;
                localVarianceFactor *= m_highResidualVariancePenalty;
            } else {
                numGood++;
            }
            debug.residual.x = residual.x();
            debug.residual.y = residual.y();
            auto effectiveVariance =
                localVarianceFactor * m_measurementVarianceScaleFactor *
                newIdentificationVariancePenalty *
                (led.isBright() ? m_brightLedVariancePenalty : 1.) *
                p.beaconMeasurementVariance[index] / led.getMeasurement().area;
            debug.variance = effectiveVariance;
            meas.setVariance(effectiveVariance);

            /// Now, do the correction.
            auto model = kalman::makeAugmentedProcessModel(p.processModel,
                                                           beaconProcess);
            kalman::correct(state, model, meas);
            gotMeasurement = true;
#ifdef DEBUG_MEASUREMENT_RESIDUALS
            if (s) {
                std::cout << "M: " << debug.measurement
                          << "  R: " << debug.residual
                          << "  s2: " << debug.variance << "\n";
            }
#endif
        }
#ifdef DEBUG_MEASUREMENT_RESIDUALS
        if (s) {
            std::cout << std::endl;
        }
#endif
        if (gotMeasurement) {
            // Re-symmetrize error covariance.
            kalman::types::DimSquareMatrix<BodyState> cov =
                0.5 * p.state.errorCovariance() +
                0.5 * p.state.errorCovariance().transpose();
            p.state.errorCovariance() = cov;
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
        if (gotMeasurement) {
            m_framesWithoutUtilizedMeasurements = 0;
        } else {
            if (inBoundsID > 0) {
                /// We had a measurement, we rejected it. The problem may be the
                /// plank in our own eye, not the speck in our beacon's eye.
                m_framesWithoutUtilizedMeasurements++;
            }
        }

        return true;
    }

    LedPtrList SCAATKalmanPoseEstimator::filterLeds(
        LedPtrList const &leds, const bool skipBright, const bool skipAll,
        std::size_t &numBad, EstimatorInOutParams const &p) {
        LedPtrList ret;

        /// @todo should we be recalculating this for each beacon after each
        /// correction step? The order we filter them in is rather arbitrary...
        const Eigen::Matrix3d rotate =
            Eigen::Matrix3d(p.state.getCombinedQuaternion());

        std::copy_if(
            begin(leds), end(leds), std::back_inserter(ret), [&](Led *ledPtr) {
                auto &led = *ledPtr;

                auto id = led.getID();
                auto index = asIndex(id);

                auto &debug = p.beaconDebug[index];
                debug.seen = true;
                debug.measurement = led.getLocationForTracking();

                // Angle of emission checking
                // If we transform the emission vector into camera space, an LED
                // pointed right at the camera will be -Z. Anything with a 0 or
                // positive z component is clearly out, and realistically,
                // anything with a z component over -0.5 is probably fairly
                // oblique. We don't want to use such beacons since they can
                // easily introduce substantial error.
                double zComponent =
                    (rotate * cvToVector(p.beaconEmissionDirection[index])).z();
                if (zComponent > 0.) {
                    if (m_extraVerbose) {
                        std::cout << "Rejecting an LED at "
                                  << led.getLocationForTracking()
                                  << " claiming ID "
                                  << led.getOneBasedID().value() << std::endl;
                    }
                    /// This means the LED is pointed away from us - so we
                    /// shouldn't be able to see it.
                    led.markMisidentified();

                    /// @todo This could be a mis-identification, or it could
                    /// mean we're in a totally messed up state. Do we count
                    /// this
                    /// against ourselves?
                    numBad++;
                    return false;
                }
                if (zComponent > m_maxZComponent) {
                    /// LED is too askew of the camera to provide reliable data,
                    /// so skip it.
                    return false;
                }

                if (skipBright && led.isBright()) {
                    return false;
                }

                if (skipAll) {
                    return false;
                }

#if 0
                /// @todo For right now, if we don't have a bounding box, we're
                /// assuming it's square enough (and only testing for
                /// non-squareness on those who actually do have bounding
                /// boxes). This is very much a temporary situation.
                auto boundingBoxRatioResult = inBoundingBoxRatioRange(led);
                if (TriBool::False == boundingBoxRatioResult) {
                    /// skip non-circular blobs.
                    numBad++;
                    return false;
                }
#endif
                return true;
            });
        return ret;
    }

    SCAATKalmanPoseEstimator::TriBool
    SCAATKalmanPoseEstimator::inBoundingBoxRatioRange(Led const &led) {
        if (led.getMeasurement().knowBoundingBox) {
            auto boundingBoxRatio = led.getMeasurement().boundingBox.height /
                                    led.getMeasurement().boundingBox.width;
            if (boundingBoxRatio > m_minBoxRatio &&
                boundingBoxRatio < m_maxBoxRatio) {
                return TriBool::True;
            }
            return TriBool::False;
        }
        return TriBool::Unknown;
    }

    // The total number of frames that we can have dodgy Kalman tracking for
    // before RANSAC takes over again.
    static const std::size_t MAX_PROBATION_FRAMES = 10;

    static const std::size_t MAX_FRAMES_WITHOUT_MEASUREMENTS = 50;

    static const std::size_t MAX_FRAMES_WITHOUT_ID_BLOBS = 10;

    SCAATKalmanPoseEstimator::TrackingHealth
    SCAATKalmanPoseEstimator::getTrackingHealth() {

        auto needsReset = (m_framesInProbation > MAX_PROBATION_FRAMES) ||
                          (m_framesWithoutUtilizedMeasurements >
                           MAX_FRAMES_WITHOUT_MEASUREMENTS);
        if (needsReset) {
            return TrackingHealth::NeedsResetNow;
        }
        /// Additional case: We've been a while without blobs...
        if (m_framesWithoutIdentifiedBlobs > MAX_FRAMES_WITHOUT_ID_BLOBS) {
            // In this case, we force ransac once we get blobs once again.
            return TrackingHealth::ResetWhenBeaconsSeen;
        }
        /// Otherwise, we're doing OK!
        return TrackingHealth::Functioning;
    }
} // namespace vbtracker
} // namespace osvr
