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
#include "ConfigParams.h"
#include "FlexibleUnscentedCorrect.h"
#include "IMUStateMeasurements.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>

// Standard includes
#include <iostream>
#include <memory>

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_CONSOLE_WIDTH 120
#include "catch_typelist.h"
#include <catch.hpp>

using std::unique_ptr;
using namespace osvr;
using namespace vbtracker;
using namespace Eigen;
struct TestData {
    TestData() {
        /// Set up from a default configuration.
        ConfigParams params;
#if 0
        state.setErrorCovariance(
            kalman::types::DimVector<BodyState>::Constant(100).asDiagonal());
#endif

        originalStateError = state.errorCovariance();
        /// configure the process model
        processModel.setDamping(params.linearVelocityDecayCoefficient,
                                params.angularVelocityDecayCoefficient);
        processModel.setNoiseAutocorrelation(
            kalman::types::Vector<6>::Map(params.processNoiseAutocorrelation));

        /// Save the IMU measurement parameters.
        imuVariance = Eigen::Vector3d::Constant(params.imu.orientationVariance);
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /// Quantity computed from the "camera pose" (room calibration output)
    Eigen::Quaterniond roomToCameraRotation = Eigen::Quaterniond::Identity();

    /// Quantity computed during room calibration output
    util::Angle yawCorrection = 0;

    /// Transform the IMU-measured quaternion using the calibration output
    /// mocked up in this struct.
    Eigen::Quaterniond xform(Eigen::Quaterniond const &quat) {
        return getTransformedOrientation(quat, roomToCameraRotation,
                                         yawCorrection);
    }

    /// Default constructor is no translation, identity rotation, 10 along
    /// main diagonal as the state covariance matrix.
    BodyState state;
    kalman::types::DimSquareMatrix<BodyState> originalStateError;

    BodyProcessModel processModel;
    Eigen::Vector3d imuVariance;
};
namespace Eigen {
inline void outputQuat(std::ostream &os, Quaterniond const &q) {
    os << "[" << q.w() << " (" << q.vec().transpose() << ")]";
}
std::ostream &operator<<(std::ostream &os, Quaterniond const &q) {
    outputQuat(os, q);
    return os;
}
} // namespace Eigen

namespace Catch {
template <> struct TypelistTypeNameTrait<kalman::QFirst> {
    static const char *get() { return "kalman::QFirst"; }
};
template <> struct TypelistTypeNameTrait<kalman::QLast> {
    static const char *get() { return "kalman::QLast"; }
};
template <> struct TypelistTypeNameTrait<kalman::SplitQ> {
    static const char *get() { return "kalman::SplitQ"; }
};
} // namespace Catch

static const double SMALL_VALUE = 0.1;

template <typename MeasurementType, typename InProgressType>
inline void commonSmallPositiveYChecks(TestData *data,
                                       MeasurementType &kalmanMeas,
                                       InProgressType &inProgress) {

    Vector3d residual = kalmanMeas.getResidual(data->state);

    CAPTURE(residual.transpose());
    AND_THEN("residual directly computed by measurement should be 0, "
             "SMALL_VALUE, 0") {
        CHECK(residual.isApprox(Vector3d(0, SMALL_VALUE, 0)));
    }

    CAPTURE(inProgress.deltaz.transpose());
    AND_THEN("computed deltaz should equal residual") {
        CHECK(residual.isApprox(inProgress.deltaz));
    }

    AND_THEN("delta z (residual/propagated mean residual) should be "
             "approximately 0, SMALL_VALUE, 0") {
        REQUIRE(inProgress.deltaz[0] == Approx(0.));
        REQUIRE(inProgress.deltaz[1] == Approx(SMALL_VALUE));
        REQUIRE(inProgress.deltaz[2] == Approx(0.));
    }

    CAPTURE(inProgress.stateCorrection.transpose());
    AND_THEN("state correction should have a rotation component with small "
             "positive y") {
        REQUIRE(inProgress.stateCorrectionFinite);
        REQUIRE(inProgress.stateCorrection[3] == Approx(0.));
        REQUIRE(inProgress.stateCorrection[4] > 0);
#if 0
        REQUIRE(inProgress.stateCorrection[4] < SMALL_VALUE);
#endif
        REQUIRE(inProgress.stateCorrection[5] == Approx(0.));
    }

    AND_THEN("state correction should have an angular velocity component "
             "with zero or small positive y") {
        REQUIRE(inProgress.stateCorrection[9] == Approx(0.));
        REQUIRE(inProgress.stateCorrection[10] >= 0.);
        REQUIRE(inProgress.stateCorrection[11] == Approx(0.));
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

#if 1

template <typename MeasurementType>
inline void smallPositiveYChecks(TestData *data, MeasurementType &kalmanMeas) {
    using JacobianType = typename MeasurementType::JacobianType;

    JacobianType jacobian = kalmanMeas.getJacobian(data->state);
    AND_THEN("the jacobian should be finite") {
        INFO("jacobian\n" << jacobian);
        REQUIRE(jacobian.allFinite());
        AND_THEN("the jacobian should not be zero") {
            REQUIRE_FALSE(jacobian.isApproxToConstant(0));
        }
        auto inProgress = kalman::beginCorrection(
            data->state, data->processModel, kalmanMeas);
        commonSmallPositiveYChecks(data, kalmanMeas, inProgress);
    }
}

CATCH_TYPELIST_TESTCASE("identity calibration output", kalman::QFirst,
                        kalman::QLast, kalman::SplitQ) {
    using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    using JacobianType = typename MeasurementType::JacobianType;
    unique_ptr<TestData> data(new TestData);
    GIVEN("an identity state") {
        WHEN("filtering in an identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should equal the measurement") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                AND_THEN("residual should be zero") {
                    CAPTURE(kalmanMeas.getResidual(data->state));
                    REQUIRE(kalmanMeas.getResidual(data->state)
                                .isApproxToConstant(0.));
                }

                JacobianType jacobian = kalmanMeas.getJacobian(data->state);
                AND_THEN("the jacobian should be finite") {
                    INFO("jacobian\n" << jacobian);
                    REQUIRE(jacobian.allFinite());
                    AND_THEN("the jacobian should not be zero") {
                        REQUIRE_FALSE(jacobian.isApproxToConstant(0));
                    }
                    auto correctionInProgress = kalman::beginCorrection(
                        data->state, data->processModel, kalmanMeas);
                    AND_THEN("computed deltaz should be zero") {
                        CAPTURE(correctionInProgress.deltaz.transpose());
                        REQUIRE(
                            correctionInProgress.deltaz.isApproxToConstant(0.));
                    }
                    CAPTURE(correctionInProgress.stateCorrection.transpose());
                    AND_THEN("state correction should be finite") {
                        REQUIRE(correctionInProgress.stateCorrectionFinite);
                        AND_THEN("state correction should be zero") {
                            REQUIRE(correctionInProgress.stateCorrection
                                        .isApproxToConstant(0.));
                        }
                    }
                }
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
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about y") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitY()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                stateRotation *
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about x") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitX()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                stateRotation *
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
}

#endif
template <typename GeneratorType>
inline void nonNegativeWeights(GeneratorType const &gen) {
    CAPTURE(gen.getWeightsForMean().transpose());
    // CHECK((gen.getWeightsForMean().array() >= 0.).all());

    CAPTURE(gen.getWeightsForCov().transpose());
    // CHECK((gen.getWeightsForCov().array() >= 0.).all());
}

template <typename GeneratorType>
inline void thenRequireNonNegativeWeights(GeneratorType const &gen) {
    THEN("Weights should be non-negative") { nonNegativeWeights(gen); }
}
template <typename GeneratorType>
inline void andThenRequireNonNegativeWeights(GeneratorType const &gen) {
    AND_THEN("Weights should be non-negative") { nonNegativeWeights(gen); }
}

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
                AND_THEN("residual should be zero") {
                    CAPTURE(kalmanMeas.getResidual(data->state));
                    REQUIRE(kalmanMeas.getResidual(data->state)
                                .isApproxToConstant(0.));
                }
                auto inProgress = kalman::beginUnscentedCorrection(
                    data->state, kalmanMeas, params);

                andThenRequireNonNegativeWeights(inProgress.sigmaPoints);
                AND_THEN("transformed sigma points should not all equal 0") {
                    REQUIRE_FALSE(
                        inProgress.transformedPoints.isApproxToConstant(0.));
                }
                auto &recon = inProgress.reconstruction;
                AND_THEN("propagated mean residual should be zero") {
                    CAPTURE(recon.getMean().transpose());
                    REQUIRE(recon.getMean().isApproxToConstant(0.));
                }
                AND_THEN(
                    "state correction should be finite - specifically, zero") {
                    CAPTURE(inProgress.stateCorrection.transpose());
                    REQUIRE(inProgress.stateCorrection.array().allFinite());
                    REQUIRE(inProgress.stateCorrection.isApproxToConstant(0));
                }
                AND_WHEN("the correction is applied") {
                    inProgress.finishCorrection();
                    THEN("state should be 0 - unchanged") {
                        CAPTURE(data->state.stateVector().transpose());
                        REQUIRE(
                            data->state.stateVector().isApproxToConstant(0));
                        CAPTURE(data->state.getQuaternion());
                        REQUIRE(data->state.getQuaternion().coeffs().isApprox(
                            Quaterniond::Identity().coeffs()));
                    }
                    THEN("State error should have decreased") {
                        REQUIRE((data->state.errorCovariance().array() <=
                                 data->originalStateError.array())
                                    .all());
                    }
                }
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