/** @file
    @brief Header

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

#ifndef INCLUDED_TestIMU_Common_h_GUID_F125CC83_F6D6_4EA5_9281_0DBEC87D309D
#define INCLUDED_TestIMU_Common_h_GUID_F125CC83_F6D6_4EA5_9281_0DBEC87D309D

// Internal Includes
#include "TestIMU.h"

#include "ConfigParams.h"
#include "IMUStateMeasurements.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>

// Standard includes
#include <iostream>
#include <memory>

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
inline std::ostream &operator<<(std::ostream &os, Quaterniond const &q) {
    outputQuat(os, q);
    return os;
}
} // namespace Eigen

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
        REQUIRE(inProgress.stateCorrection[4] < SMALL_VALUE);
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
        REQUIRE(inProgress.stateCorrection.template head<3>().isZero());
        REQUIRE(inProgress.stateCorrection.template segment<3>(6).isZero());
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
inline void smallPositiveYChecks(TestData *data, MeasurementType &kalmanMeas) {
    using JacobianType = typename MeasurementType::JacobianType;

    JacobianType jacobian = kalmanMeas.getJacobian(data->state);
    AND_THEN("the jacobian should be finite") {
        INFO("jacobian\n" << jacobian);
        REQUIRE(jacobian.allFinite());
        AND_THEN("the jacobian should not be zero") {
            REQUIRE_FALSE(jacobian.isZero());
        }
        auto inProgress = kalman::beginCorrection(
            data->state, data->processModel, kalmanMeas);
        commonSmallPositiveYChecks(data, kalmanMeas, inProgress);
    }
}

#endif // INCLUDED_TestIMU_Common_h_GUID_F125CC83_F6D6_4EA5_9281_0DBEC87D309D
