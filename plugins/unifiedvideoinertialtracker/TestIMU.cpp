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
#include "IMUStateMeasurements.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>

// Standard includes
#include <iostream>
#include <memory>

#define CATCH_CONFIG_MAIN
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
template <typename MeasurementType>
inline void smallPositiveYChecks(TestData *data, MeasurementType &kalmanMeas) {
    using JacobianType = typename MeasurementType::JacobianType;
    Vector3d residual = kalmanMeas.getResidual(data->state);
    AND_THEN("residual should be 0, SMALL_VALUE, 0") {
        CAPTURE(residual.transpose());
        CHECK(residual.isApprox(Vector3d(0, SMALL_VALUE, 0)));
    }

    JacobianType jacobian = kalmanMeas.getJacobian(data->state);
    AND_THEN("the jacobian should be finite") {
        INFO("jacobian\n" << jacobian);
        REQUIRE(jacobian.allFinite());
        AND_THEN("the jacobian should not be zero") {
            REQUIRE_FALSE(jacobian.isApproxToConstant(0));
        }
    }

    auto correctionInProgress =
        kalman::beginCorrection(data->state, data->processModel, kalmanMeas);
    AND_THEN("computed deltaz should equal residual") {
        CAPTURE(residual.transpose());
        CAPTURE(correctionInProgress.deltaz.transpose());
        CHECK(residual.isApprox(correctionInProgress.deltaz));
    }
    CAPTURE(correctionInProgress.stateCorrection.transpose());
    AND_THEN("state correction should be finite") {
        REQUIRE(correctionInProgress.stateCorrectionFinite);
        AND_THEN("state correction should have a rotation component with small "
                 "positive y") {
            REQUIRE(correctionInProgress.stateCorrection[3] == Approx(0.));
            REQUIRE(correctionInProgress.stateCorrection[4] > 0);
            REQUIRE(correctionInProgress.stateCorrection[5] == Approx(0.));
        }
        AND_THEN("state correction should have an angular velocity component "
                 "with zero or small positive y") {
            REQUIRE(correctionInProgress.stateCorrection[9] == Approx(0.));
            REQUIRE(correctionInProgress.stateCorrection[10] >= 0.);
            REQUIRE(correctionInProgress.stateCorrection[11] == Approx(0.));
        }
        AND_THEN("state correction should not contain any translational/linear "
                 "velocity components") {
            REQUIRE(correctionInProgress.stateCorrection.head<3>()
                        .isApproxToConstant(0.));
            REQUIRE(correctionInProgress.stateCorrection.segment<3>(6)
                        .isApproxToConstant(0.));
        }
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

TEST_CASE("Sigma point reconstruction validity") {
    using namespace osvr::kalman;
    Matrix3d cov(Vector3d::Constant(10).asDiagonal());
    using Generator = SigmaPointGenerator<3>;
    using Reconstructor = ReconstructedDistributionFromSigmaPoints<3, 3>;
    const auto params = SigmaPointParameters(3);
    WHEN("Starting with a zero mean") {
        Vector3d mean = Vector3d::Zero();
        auto gen = Generator(mean, cov, params);
        auto recon = Reconstructor(gen, gen.getSigmaPoints());
        THEN("the reconstructed distribution should be approximately equal to "
             "the input") {
            CAPTURE(gen.getSigmaPoints());
            CAPTURE(recon.getMean());
            REQUIRE(recon.getMean().isApproxToConstant(0));
            CAPTURE(recon.getCov());
            REQUIRE(recon.getCov().isApprox(cov));
        }
    }
}
