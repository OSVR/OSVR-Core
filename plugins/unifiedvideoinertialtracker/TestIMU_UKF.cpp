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

inline void checkReconstruction(Vector3d const &mean, Matrix3d const &cov) {
    static const auto DIM = 3;
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
}

TEST_CASE("Sigma point reconstruction validity") {
    using namespace osvr::kalman;
    Matrix3d cov(Vector3d::Constant(10).asDiagonal());
    WHEN("Starting with a zero mean") {
        checkReconstruction(Vector3d::Zero(), cov);
    }

    WHEN("Starting with a non-zero mean") {
        checkReconstruction(Vector3d(1, 2, 3), cov);
    }
    WHEN("Starting with a small non-zero mean") {
        checkReconstruction(Vector3d(0.1, 0.2, 0.3), cov);
    }
}

template <typename MeasurementType>
inline void unscentedSmallPositiveYChecks(TestData *data,
                                          MeasurementType &kalmanMeas) {
    const auto params = kalman::SigmaPointParameters();
    auto inProgress =
        kalman::beginUnscentedCorrection(data->state, kalmanMeas, params);

    AND_THEN("transformed sigma points should not all equal 0") {
        CAPTURE(inProgress.transformedPoints.transpose());
        REQUIRE_FALSE(inProgress.transformedPoints.isApproxToConstant(0.));
    }
    andThenRequireNonNegativeWeights(inProgress.sigmaPoints);

    commonSmallPositiveYChecks(data, kalmanMeas, inProgress);
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

CATCH_TYPELIST_TESTCASE("unscented with identity calibration output",
                        /*kalman::QFirst, kalman::QLast,*/ kalman::SplitQ) {

#if 0
TEST_CASE("unscented with identity calibration output") {
    using TypeParam = kalman::IMUOrientationMeasForUnscented;
#endif

    const auto params = kalman::SigmaPointParameters();
    using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    // using MeasurementType = TypeParam;
    unique_ptr<TestData> data(new TestData);
    GIVEN("an identity state") {
        WHEN("filtering in an identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should equal the measurement") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                checkEffectiveIdentityMeasurement(data.get(), kalmanMeas);
            }
        }

        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY(
                AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));
                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                unscentedSmallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about y") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitY()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY())) *
                stateRotation;
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                unscentedSmallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about x") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitX()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY())) *
                stateRotation;
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                CAPTURE(kalmanMeas.getResidual(data->state));
                unscentedSmallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
}

TEST_CASE("unscented with small x rotation calibration output") {
    using MeasurementType = OrientationMeasurementUsingPolicy<kalman::SplitQ>;
    unique_ptr<TestData> data(new TestData);
    CAPTURE(SMALL_VALUE);
    data->roomToCameraRotation =
        Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitX()));
    CAPTURE(data->roomToCameraRotation);
    GIVEN("an identity state in camera space") {
        CAPTURE(data->state.getQuaternion());
        WHEN("filtering in a measurement that's the inverse of the room to "
             "camera rotation (a measurement matching state)") {
            Quaterniond xformedMeas =
                data->xform(data->roomToCameraRotation.inverse());

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

        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            Quaterniond expectedXformedSmallPositiveRotationAboutY(
                (Isometry3d(smallPositiveRotationAboutY) *
                 Isometry3d(data->state.getQuaternion()))
                    .rotation());
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal a manually "
                 "transformed "
                 "measurement") {
                REQUIRE(xformedMeas.isApprox(
                    expectedXformedSmallPositiveRotationAboutY));
                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                CAPTURE(kalmanMeas.getResidual(data->state));
                unscentedSmallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
}
