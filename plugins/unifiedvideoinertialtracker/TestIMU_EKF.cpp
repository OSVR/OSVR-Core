/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
// Copyright 2019 Collabora, Ltd.
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

// Library/third-party includes
// - none

// Standard includes
// - none

#if 0
/// Turns out that QFirst and QLast are likely wrong.
TEMPLATE_TEST_CASE("identity calibration output", "[.][ekf]", kalman::QFirst,
                        kalman::QLast, kalman::SplitQ) {
#endif
TEMPLATE_TEST_CASE("identity calibration output", "[.][ekf]", kalman::SplitQ,
                   kalman::QLastWithSplitInnovation) {
    using MeasurementType = OrientationMeasurementUsingPolicy<TestType>;
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
        Quaterniond stateRotation(AngleAxisd(EIGEN_PI / 4., Vector3d::UnitY()));
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
        Quaterniond stateRotation(AngleAxisd(EIGEN_PI / 4., Vector3d::UnitX()));
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
