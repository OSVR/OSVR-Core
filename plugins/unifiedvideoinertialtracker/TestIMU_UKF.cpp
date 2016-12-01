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
#include "TestIMU_Common.h"

#include "FlexibleUnscentedCorrect.h"

// Library/third-party includes
// - none

// Standard includes
// - none

#ifdef DEMAND_NONNEGATIVE_WEIGHTS
template <typename GeneratorType>
inline void nonNegativeWeights(GeneratorType const &gen) {
    CAPTURE(gen.getWeightsForMean().transpose());
    CHECK((gen.getWeightsForMean().array() >= 0.).all());

    CAPTURE(gen.getWeightsForCov().transpose());
    CHECK((gen.getWeightsForCov().array() >= 0.).all());
}

template <typename GeneratorType>
inline void thenRequireNonNegativeWeights(GeneratorType const &gen) {
    THEN("Weights should be non-negative") { nonNegativeWeights(gen); }
}
template <typename GeneratorType>
inline void andThenRequireNonNegativeWeights(GeneratorType const &gen) {
    AND_THEN("Weights should be non-negative") { nonNegativeWeights(gen); }
}
#else
template <typename GeneratorType>
inline void thenRequireNonNegativeWeights(GeneratorType const &gen) {
    // not actually a requirement, or even common, it turns out.
}
template <typename GeneratorType>
inline void andThenRequireNonNegativeWeights(GeneratorType const &gen) {
    // not actually a requirement, or even common, it turns out.
}
#endif


TEST_CASE("Sigma point reconstruction validity") {

    static const auto DIM = 3;
    using namespace osvr::kalman;
    Matrix3d cov(Vector3d::Constant(10).asDiagonal());

    auto checkReconstruction = [&](Eigen::Vector3d const &mean) {
        using namespace osvr::kalman;
        using Generator = SigmaPointGenerator<DIM>;
        using Reconstructor =
            ReconstructedDistributionFromSigmaPoints<DIM, Generator>;
        const auto params = SigmaPointParameters();
        CAPTURE(mean);
        CAPTURE(cov);
        auto gen = Generator(mean, cov, params);
        thenRequireNonNegativeWeights(gen);
#if 0
        static const auto numSigmaPoints = Generator::NumSigmaPoints;
        for (std::size_t i = 0; i < numSigmaPoints; ++i) {
            CAPTURE(gen.getSigmaPoint(i));
            CHECK(false);
        }
#endif
        AND_WHEN("reconstructed from the sigma points") {
            auto recon = Reconstructor(gen, gen.getSigmaPoints());
            THEN("the reconstructed distribution should be approximately equal "
                 "to the input") {
                CAPTURE(gen.getSigmaPoints());
                CAPTURE(recon.getMean());
                for (std::size_t i = 0; i < DIM; ++i) {
                    CAPTURE(i);
                    REQUIRE(recon.getMean()[i] == Approx(mean[i]));
                }
                CAPTURE(recon.getCov());
                REQUIRE(recon.getCov().isApprox(cov));
            }
        }
    };

    WHEN("Starting with a zero mean") { checkReconstruction(Vector3d::Zero()); }

    WHEN("Starting with a non-zero mean") {
        checkReconstruction(Vector3d(1, 2, 3));
    }
    WHEN("Starting with a small non-zero mean") {
        checkReconstruction(Vector3d(0.1, 0.2, 0.3));
    }
}

enum class Axis : std::size_t { X = 0, Y = 1, Z = 2 };

inline double zeroOrValueForAxis(Axis rotationAxis, Axis currentAxis,
                                 double value) {
    return ((rotationAxis == currentAxis) ? value : 0.);
}

inline Vector3d ZeroVec3dExceptAtAxis(Axis rotationAxis, double value) {
    return Vector3d(zeroOrValueForAxis(rotationAxis, Axis::X, value),
                    zeroOrValueForAxis(rotationAxis, Axis::Y, value),
                    zeroOrValueForAxis(rotationAxis, Axis::Z, value));
}

inline Vector3d Vec3dSmallValueAt(Axis rotationAxis) {
    return ZeroVec3dExceptAtAxis(rotationAxis, SMALL_VALUE);
}

inline Vector3d Vec3dUnit(Axis rotationAxis) {
    return ZeroVec3dExceptAtAxis(rotationAxis, 1.);
}

inline double getSignCorrect(bool positive) { return (positive ? 1. : -1.); }

template <typename MeasurementType, typename InProgressType>
inline void
commonSmallSingleAxisChecks(TestData *data, MeasurementType &kalmanMeas,
                            InProgressType &inProgress, Axis const rotationAxis,
                            bool const positive = true) {
    const double signCorrect = getSignCorrect(positive);
    Vector3d rotationVector = Vec3dSmallValueAt(rotationAxis) * signCorrect;
    CAPTURE(rotationVector.transpose());

    Vector3d residual = kalmanMeas.getResidual(data->state);
    CAPTURE(residual.transpose());

    AND_THEN("residual directly computed by measurement should be zero, except "
             "for the single axis of rotation, which should be of magnitude "
             "SMALL_VALUE") {
        CHECK(residual.isApprox(rotationVector));
    }

    CAPTURE(inProgress.deltaz);
    AND_THEN("computed deltaz should equal residual") {
        CHECK(residual.isApprox(inProgress.deltaz));
    }

    AND_THEN("delta z (residual/propagated mean residual) should be "
             "approximately 0, except for the single axis of rotation, which "
             "should be magnitude SMALL_VALUE") {
        REQUIRE(inProgress.deltaz[0] == Approx(rotationVector[0]));
        REQUIRE(inProgress.deltaz[1] == Approx(rotationVector[1]));
        REQUIRE(inProgress.deltaz[2] == Approx(rotationVector[2]));
    }

    CAPTURE(inProgress.stateCorrection.transpose());
    REQUIRE(inProgress.stateCorrectionFinite);
    AND_THEN("state correction should have a rotation component with small "
             "absolute element (sign matching rotation vector) corresponding "
             "to rotation axis") {
        for (std::size_t i = 0; i < 3; ++i) {
            const auto stateIndex = i + 3;
            CAPTURE(stateIndex);
            if (static_cast<std::size_t>(rotationAxis) == i) {
                /// This is our rotation axis - correction should be in (0,
                /// SMALL_VALUE)
                REQUIRE((signCorrect * inProgress.stateCorrection[stateIndex]) >
                        0);
                REQUIRE((signCorrect * inProgress.stateCorrection[stateIndex]) <
                        SMALL_VALUE);
            } else {
                /// Not our rotation axis, correction should be approx 0.
                REQUIRE(inProgress.stateCorrection[stateIndex] == Approx(0.));
            }
        }
    }

    AND_THEN("state correction should have an angular velocity component "
             "with zero or small abs (sign matching rotation) corresponding to "
             "rotation axis") {

        for (std::size_t i = 0; i < 3; ++i) {
            const auto stateIndex = i + 9;
            CAPTURE(stateIndex);
            if (static_cast<std::size_t>(rotationAxis) == i) {
                /// This is our rotation axis - correction should be >= 0 if
                /// positive rotation.
                REQUIRE(signCorrect * inProgress.stateCorrection[stateIndex] >=
                        0.);
            } else {
                /// Not our rotation axis, correction should be approx 0.
                REQUIRE(inProgress.stateCorrection[stateIndex] == Approx(0.));
            }
        }
    }
    AND_THEN("state correction should not contain any translational/linear "
             "velocity components") {
        REQUIRE(inProgress.stateCorrection.head<3>().isApproxToConstant(0.));
        REQUIRE(
            inProgress.stateCorrection.segment<3>(6).isApproxToConstant(0.));
    }
    AND_WHEN("the correction is applied") {
        auto errorCovarianceCorrectionWasFinite = inProgress.finishCorrection();
        THEN("the new error covariance should be finite") {
            REQUIRE(errorCovarianceCorrectionWasFinite);
            AND_THEN("State error should have decreased") {
                REQUIRE((data->state.errorCovariance().array() <=
                         data->originalStateError.array())
                            .all());
            }
        }
    }
}

template <typename MeasurementType>
inline void
unscentedSmallSingleAxisChecks(TestData *data, MeasurementType &kalmanMeas,
                               Axis rotationAxis, bool positive = true) {
    const auto params = kalman::SigmaPointParameters();
    auto inProgress =
        kalman::beginUnscentedCorrection(data->state, kalmanMeas, params);

    AND_THEN("transformed sigma points should not all equal 0") {
        CAPTURE(inProgress.transformedPoints.transpose());
        REQUIRE_FALSE(inProgress.transformedPoints.isApproxToConstant(0.));
    }
    andThenRequireNonNegativeWeights(inProgress.sigmaPoints);

    commonSmallSingleAxisChecks(data, kalmanMeas, inProgress, rotationAxis,
                                positive);
}

template <typename MeasurementType>
inline void checkEffectiveIdentityMeasurement(TestData *data,
                                              MeasurementType &kalmanMeas) {
    AND_THEN("residual should be zero") {
        CAPTURE(kalmanMeas.getResidual(data->state));
        REQUIRE(kalmanMeas.getResidual(data->state).isApproxToConstant(0.));
    }
    auto inProgress = kalman::beginUnscentedCorrection(data->state, kalmanMeas);

    andThenRequireNonNegativeWeights(inProgress.sigmaPoints);
    AND_THEN("transformed sigma points should not all equal 0") {
        REQUIRE_FALSE(inProgress.transformedPoints.isApproxToConstant(0.));
    }
    auto &recon = inProgress.reconstruction;
    AND_THEN("propagated predicted measurement mean should be zero") {
        CAPTURE(recon.getMean().transpose());
        REQUIRE(recon.getMean().isApproxToConstant(0.));
    }
    AND_THEN("state correction should be finite - specifically, zero") {
        CAPTURE(inProgress.stateCorrection.transpose());
        REQUIRE(inProgress.stateCorrection.array().allFinite());
        REQUIRE(inProgress.stateCorrection.isApproxToConstant(0));
    }

    const Quaterniond origQuat = data->state.getQuaternion();
    AND_WHEN("the correction is applied") {
        inProgress.finishCorrection();
        THEN("state should be unchanged") {
            CAPTURE(data->state.stateVector().transpose());
            REQUIRE(data->state.stateVector().isApproxToConstant(0));
            CAPTURE(origQuat);
            CAPTURE(data->state.getQuaternion());
            REQUIRE(data->state.getQuaternion().coeffs().isApprox(
                origQuat.coeffs()));
        }
        THEN("State error should have decreased") {
            REQUIRE((data->state.errorCovariance().array() <=
                     data->originalStateError.array())
                        .all());
        }
    }
}

template <typename F, typename... Args>
inline void allSmallSingleAxisRotations(F &&f, Args &&... args) {
    WHEN("filtering in a small positive rotation about x") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::X, true);
    }

    WHEN("filtering in a small positive rotation about y") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::Y, true);
    }

    WHEN("filtering in a small positive rotation about z") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::Z, true);
    }

    WHEN("filtering in a small negative rotation about x") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::X, false);
    }
    WHEN("filtering in a small negative rotation about y") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::Y, false);
    }

    WHEN("filtering in a small negative rotation about z") {
        std::forward<F>(f)(std::forward<Args>(args)..., Axis::Z, false);
    }
}

#if 0
CATCH_TYPELIST_DESCRIBED_TESTCASE(
    "unscented with identity calibration output", "[ukf]",
    /*kalman::QFirst, kalman::QLast,*/ kalman::SplitQ) {

#endif
TEST_CASE("unscented with identity calibration output", "[ukf]") {
    using TypeParam = kalman::SplitQ;

    const auto params = kalman::SigmaPointParameters();
    using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    // using MeasurementType = TypeParam;
    unique_ptr<TestData> data(new TestData);
    GIVEN("an identity state") {
        WHEN("filtering in an identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should equal the "
                 "measurement") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                checkEffectiveIdentityMeasurement(data.get(), kalmanMeas);
            }
        }
        allSmallSingleAxisRotations([&](Axis rotationAxis, bool positive) {
            const double signCorrect = getSignCorrect(positive);
            auto radians = signCorrect * SMALL_VALUE;
            CAPTURE(radians);
            auto axisVec = Vec3dUnit(rotationAxis);
            CAPTURE(axisVec.transpose());
            Quaterniond smallRotation =
                Quaterniond(AngleAxisd(radians, axisVec));

            CAPTURE(smallRotation);
            Quaterniond xformedMeas = data->xform(smallRotation);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the "
                 "measurement") {
                REQUIRE(xformedMeas.isApprox(smallRotation));
                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                unscentedSmallSingleAxisChecks(data.get(), kalmanMeas,
                                               rotationAxis, positive);
            }
        });
    }

    auto runIncrementalSmallRotChecksNonIdentityState = [&](Axis rotationAxis,
                                                            bool positive) {
        const double signCorrect = getSignCorrect(positive);
        auto radians = signCorrect * SMALL_VALUE;
        CAPTURE(radians);
        auto axisVec = Vec3dUnit(rotationAxis);
        CAPTURE(axisVec.transpose());
        Quaterniond smallRotation = Quaterniond(AngleAxisd(radians, axisVec)) *
                                    data->state.getQuaternion();

        CAPTURE(smallRotation);
        Quaterniond xformedMeas = data->xform(smallRotation);
        CAPTURE(xformedMeas);

        THEN("the transformed measurement should equal the "
             "measurement") {
            REQUIRE(xformedMeas.isApprox(smallRotation));
            /// Do the rest of the checks for a small rotation about y
            MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
            unscentedSmallSingleAxisChecks(data.get(), kalmanMeas, rotationAxis,
                                           positive);
        }
    };
    GIVEN("a state rotated about y") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitY()));
        data->state.setQuaternion(stateRotation);
        allSmallSingleAxisRotations(
            runIncrementalSmallRotChecksNonIdentityState);
    }
    GIVEN("a state rotated about x") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitX()));
        data->state.setQuaternion(stateRotation);
        allSmallSingleAxisRotations(
            runIncrementalSmallRotChecksNonIdentityState);
    }
}

TEST_CASE("unscented with small x rotation calibration output", "[ukf]") {
    using MeasurementType = OrientationMeasurementUsingPolicy<kalman::SplitQ>;
    unique_ptr<TestData> data(new TestData);
    CAPTURE(SMALL_VALUE);
    data->roomToCameraRotation =
        Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitX()));
    CAPTURE(data->roomToCameraRotation);
    INFO("This calibration value means that the camera is rotated negative "
         "about its x wrt. the IMU/room");
    INFO("Equivalently, that quaternion calibration value takes "
         "points/orientations in room space and moves them to camera space");
    GIVEN("an identity state in camera space") {
        CAPTURE(data->state.getQuaternion());
        WHEN("filtering in a measurement that's the inverse of the room to "
             "camera rotation (a measurement matching state)") {
            Quaterniond meas = data->roomToCameraRotation.inverse();
            CAPTURE(meas);
            Quaterniond xformedMeas = data->xform(meas);

            THEN("the transformed measurement should be approximately the "
                 "identity") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                checkEffectiveIdentityMeasurement(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("an identity state in room space") {
        data->state.setQuaternion(data->roomToCameraRotation);
        CAPTURE(data->state.getQuaternion());
        WHEN("filtering in an identity measurement (a measurement matching "
             "state)") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should be approximately the "
                 "room to camera rotation") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(data->roomToCameraRotation));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                checkEffectiveIdentityMeasurement(data.get(), kalmanMeas);
            }
        }
        allSmallSingleAxisRotations([&](Axis rotationAxis, bool positive) {
            const double signCorrect = getSignCorrect(positive);
            auto radians = signCorrect * SMALL_VALUE;
            CAPTURE(radians);
            auto axisVec = Vec3dUnit(rotationAxis);
            CAPTURE(axisVec.transpose());
            Quaterniond smallRelRotationInCameraSpace =
                Quaterniond(AngleAxisd(radians, axisVec)) *
                data->state.getQuaternion();
            CAPTURE(smallRelRotationInCameraSpace);
            Quaterniond smallRotation = data->roomToCameraRotation.inverse() *
                                        smallRelRotationInCameraSpace;
            CAPTURE(smallRotation);
            THEN("the transformed measurement should equal the original "
                 "measurement as computed in camera space") {
                Quaterniond xformedMeas = data->xform(smallRotation);
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(smallRelRotationInCameraSpace));

                /// Do the rest of the checks for a small rotation about a
                /// single axis
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                CAPTURE(kalmanMeas.getResidual(data->state));
                unscentedSmallSingleAxisChecks(data.get(), kalmanMeas,
                                               rotationAxis, positive);
            }
        });
    }
}

TEST_CASE("conceptual transformation orders") {
    Quaterniond positiveX(AngleAxisd(0.5, Vector3d::UnitX()));
    Quaterniond positiveY(AngleAxisd(0.5, Vector3d::UnitY()));
    Vector3d yVec = Vector3d::UnitY();
    SECTION(
        "Quaternion composition/multiplication order matches "
        "transformation order (transformation and quat mult are associative)") {
        CAPTURE((positiveX * (positiveY * yVec)).transpose());
        CAPTURE(((positiveX * positiveY) * yVec).transpose());
        REQUIRE((positiveX * (positiveY * yVec))
                    .isApprox((positiveX * positiveY) * yVec));
    }
    SECTION("Behavior of rotations") {
        INFO("positive X is a transformation taking points from a coordinate "
             "system pitched up, to a level coordinate system.");
        CAPTURE((positiveX * yVec).transpose());
        REQUIRE((positiveX * yVec).z() > 0.);
    }
}
