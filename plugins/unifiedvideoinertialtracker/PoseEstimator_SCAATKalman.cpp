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

#include <iostream>
#include <osvr/Util/EigenCoreGeometry.h>
#include <util/Stride.h>
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
#include "ImagePointMeasurement.h"
#include "LED.h"
#include "PinholeCameraFlip.h"
#include "PoseEstimator_SCAATKalman.h"
#include "UsefulQuaternions.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <osvr/Kalman/AugmentedProcessModel.h>
#include <osvr/Kalman/AugmentedState.h>
#include <osvr/Kalman/ConstantProcess.h>
#include <osvr/Kalman/FlexibleKalmanFilter.h>

#include <util/Stride.h>

// Standard includes
#include <algorithm>
#include <iostream>
#include <iterator> // back_inserter
#include <random>

#undef OSVR_DEBUG_VELOCITY
#undef OSVR_VARIANCE_PENALTY_FOR_FIXED_BEACONS
#undef OSVR_CHECK_BOUNDING_BOXES
#undef OSVR_TRY_LIMITING_ANGULAR_VELOCITY_CHANGE
#undef OSVR_DEBUG_EMISSION_DIRECTION

namespace osvr {
namespace vbtracker {
    static const auto DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS = 4;

    /// If we have more that this number of supposedly misidentifieds per their
    /// residual, and more misidentifieds than used, then we don't actually mark
    /// them misidentified, just skip them and count them as bad (which will
    /// tend to cause us to reset the model)
    static const auto MISIDENTIFIED_BEACON_CUTOFF = 3;

    /// 4, so that if the residual itself is 2x the max residual, we reject the
    /// identification.
    static const auto SQUARED_MAX_RESIDUAL_FACTOR_FOR_ID_REJECT = 4;
    SCAATKalmanPoseEstimator::SCAATKalmanPoseEstimator(
        ConfigParams const &params)
        : m_shouldSkipBright(params.shouldSkipBrightLeds),
          m_maxResidual(params.maxResidual),
          m_maxSquaredResidual(params.maxResidual * params.maxResidual),
          m_maxZComponent(params.maxZComponent),
          m_highResidualVariancePenalty(params.highResidualVariancePenalty),
          m_beaconProcessNoise(params.beaconProcessNoise),
          m_noveltyPenaltyBase(params.tuning.noveltyPenaltyBase),
          m_noBeaconLinearVelocityDecayCoefficient(
              params.noBeaconLinearVelocityDecayCoefficient),
          m_brightLedVariancePenalty(params.brightLedVariancePenalty),
          m_measurementVarianceScaleFactor(
              params.measurementVarianceScaleFactor),
          m_distanceMeasVarianceBase(params.tuning.distanceMeasVarianceBase),
          m_distanceMeasVarianceIntercept(
              params.tuning.distanceMeasVarianceIntercept),
          m_extraVerbose(params.extraVerbose),
          m_randEngine(std::random_device()()) {
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

        if (goodLeds.empty() && p.startingTime != frameTime) {
            /// If we have no good LEDs, let's attenuate the linear velocity
            /// quickly: avoid coasting/gliding.
            auto dt = util::time::duration(frameTime, p.startingTime);
            auto atten = std::pow(m_noBeaconLinearVelocityDecayCoefficient, dt);
            p.state.velocity() *= atten;
        }

        /// Shuffle the order of the good LEDS
        std::shuffle(begin(goodLeds), end(goodLeds), m_randEngine);

#if 0
        static ::util::Stride varianceStride{ 809 };
        if (++varianceStride) {
            std::cout << p.state.errorCovariance().diagonal().transpose()
                << std::endl;
        }
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
                std::pow(m_noveltyPenaltyBase, led.novelty());

            /// Stick a little bit of process model uncertainty in the beacon,
            /// if it's meant to have some
            if (p.beaconFixed[index]) {
                beaconProcess.setNoiseAutocorrelation(0);
#ifdef OSVR_VARIANCE_PENALTY_FOR_FIXED_BEACONS
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
            meas.setMeasurement(
                cvToVector(led.getLocationForTracking()).cast<double>());

            auto state =
                kalman::makeAugmentedState(p.state, *(p.beacons[index]));
            meas.updateFromState(state);

            /// Investigate measurement variance here (difference in measurement
            /// space from expected measurement based on the model) to decide if
            ///
            /// - it's a reasonable measurement
            /// - it's a little unreasonable (and should get some extra variance
            ///   to indicate we think it's a poor-quality measurement)
            /// - it's totally unreasonable and should be de-identified and
            ///   skipped (because we've probably mis-identified something in
            ///   the environment as a beacon)
            Eigen::Vector2d residual = meas.getResidual(state);
            auto squaredResidual = residual.squaredNorm();
            // Compute what the squared, pixel-space residual would be for the
            // configured, max-tolerable residual in meters at the beacon depth
            // before applying the penalty
            auto depth = meas.getBeaconInCameraSpace().z();
            auto maxSquaredResidual =
                squaredXyDistanceFromMetersToPixels(m_maxResidual, depth, cam);
            if (squaredResidual > maxSquaredResidual) {
                // OK, it's bad, but is it really bad?

                /// Have to still count it as bad, in case it's our model, not
                /// the beacon, that's actually bad.
                numBad++;

                // Let's see if it's really bad and thus likely actually some
                // other object that we've mis-recognized as a beacon, like a
                // lighthouse base station.
                if (squaredResidual >
                    squaredXyDistanceFromMetersToPixels(
                        SQUARED_MAX_RESIDUAL_FACTOR_FOR_ID_REJECT *
                            m_maxResidual,
                        depth, cam)) {
                    // Yeah, it's really bad, throw it out!
                    markAsPossiblyMisidentified(led);
                    continue;
                }

                // OK, it's just probably a low-quality measurement but
                // not a measurement of something else.
                localVarianceFactor *= m_highResidualVariancePenalty;
            } else {
                // It's reasonable!
                numGood++;
            }

            /// That was the last place we'd reject an LED, so now we can say
            /// for sure we're using this one.
            markAsUsed(led);

            debug.residual.x = residual.x();
            debug.residual.y = residual.y();
#if 0
            /// ad-hoc estimated variance computation from experimentation,
            /// tuned with optimizer and m_measurementVarianceScaleFactor
            auto effectiveVariance =
                localVarianceFactor * m_measurementVarianceScaleFactor *
                newIdentificationVariancePenalty *
                (led.isBright() ? m_brightLedVariancePenalty : 1.) *
                p.beaconMeasurementVariance[index] / led.getMeasurement().area;
#else
            /// Typically, all of these will be 1 except for the variance from
            /// beacon depth.
            /// At some distance past 55cm, bright LEDs actually have lower than
            /// average variances instead of higher, but the overall mean
            /// variance follows an exponential decay trend with distance.
            auto effectiveVariance = localVarianceFactor *
                                     m_measurementVarianceScaleFactor *
                                     newIdentificationVariancePenalty *
                                     p.beaconMeasurementVariance[index] *
                                     getVarianceFromBeaconDepth(depth);
#endif
            debug.variance = effectiveVariance;
            meas.setVariance(effectiveVariance);

            /// Now, do the correction.
            auto model = kalman::makeAugmentedProcessModel(p.processModel,
                                                           beaconProcess);

            auto correction = kalman::beginCorrection(state, model, meas);
            if (!correction.stateCorrectionFinite) {
                std::cout << "Non-finite state correction processing beacon "
                          << led.getOneBasedID().value() << std::endl;
                continue;
            }
#ifdef OSVR_TRY_LIMITING_ANGULAR_VELOCITY_CHANGE
            /// this is the velocity correction:
            /// correction.stateCorrection.segment<3>(6)
            /// this is the angular velocity correction:
            /// correction.stateCorrection.segment<3>(9)

            /// These are, in practice, surprisingly high...
            /// as well as dependent on the variances...
            static const auto MaxAngVelChangeFromOneBeacon = 3 * EIGEN_PI;
            static const auto MaxAnglVelChangeSquared =
                MaxAngVelChangeFromOneBeacon * MaxAngVelChangeFromOneBeacon;
            auto angVelChangeSquared =
                correction.stateCorrection.segment<3>(9).squaredNorm();
            if (angVelChangeSquared > MaxAnglVelChangeSquared) {
                std::cout << "Got too high of a angular velocity correction "
                             "from report from "
                          << led.getOneBasedID().value() << ": magnitude "
                          << std::sqrt(angVelChangeSquared) << std::endl;
                continue;
            }
#endif
            correction.finishCorrection();

            gotMeasurement = true;
        }

        handlePossiblyMisidentifiedLeds();

        if (gotMeasurement) {
            // Re-symmetrize error covariance.
            kalman::types::DimSquareMatrix<BodyState> cov =
                0.5 * p.state.errorCovariance() +
                0.5 * p.state.errorCovariance().transpose();
            p.state.errorCovariance() = cov;

#ifdef OSVR_DEBUG_VELOCITY
            {
                static ::util::Stride s(77);
                if (++s) {
                    if (p.state.velocity().squaredNorm() > 0.01) {
                        // fast enough to say something about
                        std::cout
                            << "Velocity: " << p.state.velocity().transpose()
                            << "\n";
                    }
                    double angSpeed = p.state.angularVelocity().norm();
                    if (angSpeed > 0.1) {
                        std::cout << "AngVel: " << angSpeed << " about "
                                  << (p.state.angularVelocity() / angSpeed)
                                         .transpose()
                                  << "\n";
                    }
                }
            }
#endif
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

        if (gotMeasurement && p.state.position().z() < 0) {
            if (m_extraVerbose) {
                std::cout << "Kalman detected wrong side of the looking glass! "
                             "We can't be behind the camera and be seen, but "
                             "our position is apparently "
                          << p.state.position().transpose() << std::endl;
            }
            /// We're on the wrong side of the camera #fail
            /// This is a fix-up after the fact, rather than a prevention: if we
            /// get here, we goofed up earlier - but better late than never.
            Eigen::Quaterniond quat = p.state.getQuaternion();
            pinholeCameraFlipPose(p.state.position(), quat);
            p.state.setQuaternion(quat);

            pinholeCameraFlipVelocities(p.state.velocity(),
                                        p.state.angularVelocity());
#if 0
            /// invert position and velocity
            p.state.position() *= -1;
            p.state.velocity() *= -1;

            /// rotate orientation/angvel 180 about z.
            p.state.angularVelocity() =
                get180aboutZ() * p.state.angularVelocity();
            p.state.setQuaternion(get180aboutZ() * p.state.getQuaternion());
#endif
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
            p.state.getCombinedQuaternion().toRotationMatrix();
        /// @todo use this for more speed?
        // Eigen::RowVector3d zRotate = rotate.row(2);

        std::copy_if(
            begin(leds), end(leds), std::back_inserter(ret), [&](Led *ledPtr) {
                auto &led = *ledPtr;

                auto id = led.getID();
                auto index = asIndex(id);

                auto &debug = p.beaconDebug[index];
                debug.seen = true;
                debug.measurement = led.getLocationForTracking();
                if (skipAll) {
                    return false;
                }

                // Angle of emission checking
                // If we transform the emission vector into camera space, an LED
                // pointed right at the camera will be -Z. Anything with a 0 or
                // positive z component is clearly out, and realistically,
                // anything with a z component over -0.5 is probably fairly
                // oblique. We don't want to use such beacons since they can
                // easily introduce substantial error.
                double zComponent =
                    (rotate * cvToVector(p.beaconEmissionDirection[index])).z();
#if OSVR_DEBUG_EMISSION_DIRECTION

                /// Beacon 32 is right on the front, should be facing nice and
                /// forward.
                // static const auto beaconInspected = OneBasedBeaconId(32);
                /// Beacon 10 is on the right side
                // static const auto beaconInspected = OneBasedBeaconId(10);
                /// Beacon 5 on on the left side
                static const auto beaconInspected = OneBasedBeaconId(5);
                /// Beacon 9 is on the top

                if (makeOneBased(id) == beaconInspected) {
                    static ::util::Stride s(20);
                    if (s) {
                        auto altZ =
                            zRotate *
                            cvToVector(p.beaconEmissionDirection[index]);
                        std::cout
                            << "Beacon " << beaconInspected.value()
                            << ": alternate z component is " << altZ
                            << " and beacon emission direction is "
                            << (rotate *
                                cvToVector(p.beaconEmissionDirection[index]))
                                   .transpose()
                            << std::endl;
                    }
                    s.advance();
                }
#endif
                if (zComponent > 0.) {
                    if (m_extraVerbose) {
                        std::cout
                            << "Rejecting an LED at "
                            << led.getLocationForTracking() << " claiming ID "
                            << led.getOneBasedID().value()
                            << " thus emission vec "
                            << (rotate *
                                cvToVector(p.beaconEmissionDirection[index]))
                                   .transpose()
                            << std::endl;
                    }
                    /// This means the LED is pointed away from us - so we
                    /// shouldn't be able to see it.
                    markAsPossiblyMisidentified(led);

                    /// @todo This could be a mis-identification, or it could
                    /// mean we're in a totally messed up state. Do we count
                    /// this against ourselves?
                    numBad++;
                    return false;
                }
                if (zComponent > m_maxZComponent) {
                    /// LED is too askew of the camera to provide reliable data,
                    /// so skip it.
                    if (m_extraVerbose) {
                        std::cout << "Skipping " << led.getOneBasedID().value()
                                  << " with z component " << zComponent
                                  << std::endl;
                    }
                    return false;
                }

                if (skipBright && led.isBright()) {
                    return false;
                }

#ifdef OSVR_CHECK_BOUNDING_BOXES
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
        if (led.getMeasurement().knowBoundingBox()) {
            auto boundingBoxRatio =
                led.getMeasurement().boundingBoxSize().height /
                led.getMeasurement().boundingBoxSize().width;
            if (boundingBoxRatio > m_minBoxRatio &&
                boundingBoxRatio < m_maxBoxRatio) {
                return TriBool::True;
            }
            return TriBool::False;
        }
        return TriBool::Unknown;
    }

    void SCAATKalmanPoseEstimator::markAsPossiblyMisidentified(Led &led) {
        m_possiblyMisidentified.push_back(&led);
    }

    void SCAATKalmanPoseEstimator::markAsUsed(Led &led) {
        led.markAsUsed();
        m_ledsUsed++;
    }

    double SCAATKalmanPoseEstimator::getVarianceFromBeaconDepth(double depth) {
        return std::pow(m_distanceMeasVarianceBase, depth * 100.) *
               m_distanceMeasVarianceIntercept;
    }

    void SCAATKalmanPoseEstimator::handlePossiblyMisidentifiedLeds() {
        m_ledsConsideredMisidentifiedLastFrame = m_possiblyMisidentified.size();
        if (m_ledsConsideredMisidentifiedLastFrame > m_ledsUsed &&
            m_ledsConsideredMisidentifiedLastFrame >
                MISIDENTIFIED_BEACON_CUTOFF) {
            std::cout << "Think our model is wrong: considered "
                      << m_ledsConsideredMisidentifiedLastFrame
                      << " beacons misidentified, while only used "
                      << m_ledsUsed << std::endl;
            m_misIDConsideredOurFault = true;
        } else {
            m_misIDConsideredOurFault = false;
            for (auto &ledPtr : m_possiblyMisidentified) {
                ledPtr->markMisidentified();
            }
        }

        // Get ready for the next frame.
        m_ledsUsedLastFrame = m_ledsUsed;
        m_ledsUsed = 0;
        m_possiblyMisidentified.clear();
    }

    // The total number of frames that we can have dodgy Kalman tracking for
    // before RANSAC takes over again.
    static const std::size_t MAX_PROBATION_FRAMES = 10;

    // static const std::size_t MAX_FRAMES_WITHOUT_MEASUREMENTS = 100;

    static const std::size_t MAX_FRAMES_WITHOUT_ID_BLOBS = 25;

    SCAATKalmanPoseEstimator::TrackingHealth
    SCAATKalmanPoseEstimator::getTrackingHealth() {
        // Reset immediately if we have a large number of mis-identified beacons
        // that we decided were a model error, not a recognition error.
        if (m_misIDConsideredOurFault) {
            return TrackingHealth::NeedsHardResetNow;
        }

        auto needsReset = (m_framesInProbation > MAX_PROBATION_FRAMES);
#if 0
			||
                          (m_framesWithoutUtilizedMeasurements >
                           MAX_FRAMES_WITHOUT_MEASUREMENTS);
#endif
        if (needsReset) {
            return TrackingHealth::NeedsHardResetNow;
        }
        /// Additional case: We've been a while without blobs...
        if (m_framesWithoutIdentifiedBlobs > MAX_FRAMES_WITHOUT_ID_BLOBS) {
            // In this case, we force ransac once we get blobs once again.
            return TrackingHealth::SoftResetWhenBeaconsSeen;
        }
        /// Otherwise, we're doing OK!
        return TrackingHealth::Functioning;
    }
} // namespace vbtracker
} // namespace osvr
