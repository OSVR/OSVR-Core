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
#include <chrono> // std::chrono::system_clock
#include <iostream>
#include <iterator> // back_inserter
#include <random>   // std::default_random_engine

#undef DEBUG_MEASUREMENT_RESIDUALS

namespace osvr {
namespace kalman {
    template <typename StateType, typename MeasurementType>
    struct CorrectionInProgress {
        /// Dimension of measurement
        static const types::DimensionType m =
            types::Dimension<MeasurementType>::value;
        /// Dimension of state
        static const types::DimensionType n =
            types::Dimension<StateType>::value;

        CorrectionInProgress(StateType &state, MeasurementType &meas,
                             types::SquareMatrix<n> const &P_,
                             types::Matrix<n, m> const &PHt_,
                             types::SquareMatrix<m> const &S)
            : P(P_), PHt(PHt_), denom(S), deltaz(meas.getResidual(state)),
              stateCorrection(PHt * denom.solve(deltaz)), state_(state) {}

        /// State error covariance
        types::SquareMatrix<n> P;

        /// The kalman gain stuff to not invert (called P12 in TAG)
        types::Matrix<n, m> PHt;

        /// Decomposition of S
        Eigen::LDLT<types::SquareMatrix<m>> denom;

        /// Measurement residual or delta z
        types::Vector<m> deltaz;

        /// Corresponding state change to apply.
        types::Vector<n> stateCorrection;

        /// That's as far as we go here before you choose to continue.

        /// Finish computing the rest and correct the state.
        void finishCorrection() {
            // Correct the state estimate
            state_.setStateVector(state_.stateVector() + stateCorrection);

            // Correct the error covariance
            // differs from the (I-KH)P form by not factoring out the P (since
            // we already have PHt computed).
            types::SquareMatrix<n> newP =
                P - (PHt * denom.solve(PHt.transpose()));
            state_.setErrorCovariance(newP);

            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state_.postCorrect();
        }

      private:
        StateType &state_;
    };

    template <typename StateType, typename ProcessModelType,
              typename MeasurementType>
    inline CorrectionInProgress<StateType, MeasurementType>
    beginCorrection(StateType &state, ProcessModelType &processModel,
                    MeasurementType &meas) {

        /// Dimension of measurement
        static const auto m = types::Dimension<MeasurementType>::value;
        /// Dimension of state
        static const auto n = types::Dimension<StateType>::value;

        /// Measurement Jacobian
        types::Matrix<m, n> H = meas.getJacobian(state);

        /// Measurement covariance
        types::SquareMatrix<m> R = meas.getCovariance(state);

        /// State error covariance
        types::SquareMatrix<n> P = state.errorCovariance();

        /// The kalman gain stuff to not invert (called P12 in TAG)
        types::Matrix<n, m> PHt = P * H.transpose();

        /// the stuff to invert for the kalman gain
        /// also sometimes called S or the "Innovation Covariance"
        types::SquareMatrix<m> S = H * PHt + R;

        /// More computation is done in initializers/constructor
        return CorrectionInProgress<StateType, MeasurementType>(state, meas, P,
                                                                PHt, S);
    }

} // namespace kalman

namespace vbtracker {

    static const auto DIM_BEACON_CUTOFF_TO_SKIP_BRIGHTS = 4;
    SCAATKalmanPoseEstimator::SCAATKalmanPoseEstimator(
        ConfigParams const &params)
        : m_shouldSkipBright(params.shouldSkipBrightLeds),
          m_maxResidual(params.maxResidual),
          m_maxSquaredResidual(params.maxResidual * params.maxResidual),
          m_maxZComponent(params.maxZComponent),
          m_highResidualVariancePenalty(params.highResidualVariancePenalty),
          m_beaconProcessNoise(params.beaconProcessNoise),
          m_noveltyPenaltyBase(params.tuning.noveltyPenaltyBase),
          m_brightLedVariancePenalty(params.brightLedVariancePenalty),
          m_measurementVarianceScaleFactor(
              params.measurementVarianceScaleFactor),
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
                std::pow(m_noveltyPenaltyBase, led.novelty());

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
            meas.setMeasurement(
                cvToVector(led.getLocationForTracking()).cast<double>());

            led.markAsUsed();
            auto state =
                kalman::makeAugmentedState(p.state, *(p.beacons[index]));
            meas.updateFromState(state);
            Eigen::Vector2d residual = meas.getResidual(state);
            auto depth = meas.getBeaconInCameraSpace().z();
            // Only tolerate a residual of 0.15m at the beacon depth.
            auto maxSquaredResidual =
                squaredXyDistanceFromMetersToPixels(m_maxResidual, depth, cam);
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

#if 0
			kalman::correct(state, model, meas);
#else
            auto correction = kalman::beginCorrection(state, model, meas);
#if 0
			/// this is the velocity correction:
			/// correction.stateCorrection.segment<3>(6)
			/// this is the angular velocity correction:
			/// correction.stateCorrection.segment<3>(9)

			/// These are, in practice, surprisingly high...
			/// as well as dependent on the variances...
            static const auto MaxAngVelChangeFromOneBeacon = 3 * M_PI;
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
#endif

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

#ifdef DEBUG_VELOCITY
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

            /// invert position and velocity
            p.state.position() *= -1;
            p.state.velocity() *= -1;

            /// rotate orientation/angvel 180 about z.
            p.state.angularVelocity() =
                get180aboutZ() * p.state.angularVelocity();
            p.state.setQuaternion(get180aboutZ() * p.state.getQuaternion());
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
#if 0

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
                    led.markMisidentified();

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
