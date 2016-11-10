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

CATCH_TYPELIST_TESTCASE("identity calibration output", kalman::QFirst,
                        kalman::QLast, kalman::SplitQ) {
    using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    using JacobianType = typename MeasurementType::JacobianType;
    unique_ptr<TestData> data(new TestData);
    SECTION("identity state") {
        SECTION("identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());
            CAPTURE(xformedMeas);
            REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));
            MeasurementType meas{xformedMeas, data->imuVariance};
            CAPTURE(meas.getResidual(data->state));
            REQUIRE(meas.getResidual(data->state).isApproxToConstant(0.));

            JacobianType jacobian = meas.getJacobian(data->state);
            INFO("jacobian\n" << jacobian);
            REQUIRE(jacobian.allFinite());
        }

        SECTION("measure small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY(
                AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);
            REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

            MeasurementType meas{xformedMeas, data->imuVariance};
            Vector3d residual = meas.getResidual(data->state);
            CAPTURE(residual.transpose());
            CHECK(residual.isApprox(Vector3d(0, SMALL_VALUE, 0)));
            JacobianType jacobian = meas.getJacobian(data->state);
            INFO("jacobian\n" << jacobian);
            REQUIRE(jacobian.allFinite());
        }
    }
}
