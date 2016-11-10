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
#include <catch.hpp>

using std::unique_ptr;
using namespace osvr;
using namespace vbtracker;
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

template<typename MeasurementType>
inline void identityMeas(TestData & data, Eigen::Quaterniond const& xformedMeas) {
	
        MeasurementType meas{xformedMeas, data.imuVariance};
        REQUIRE(meas.getResidual(data.state).isApproxToConstant(0));
        std::cout << meas.getJacobian(data.state) << std::endl;
		REQUIRE(meas.getJacobian(data.state).allFinite());
}
TEST_CASE("identity calibration output") {
    unique_ptr<TestData> data(new TestData);
    SECTION("identity measurement") {

        REQUIRE(data->xform(Eigen::Quaterniond::Identity())
                    .isApprox(Eigen::Quaterniond::Identity()));
        auto xformedMeas = data->xform(Eigen::Quaterniond::Identity());
        OrientationMeasurement meas{xformedMeas, data->imuVariance};
        REQUIRE(meas.getResidual(data->state).isApproxToConstant(0));
        //std::cout << meas.getJacobian(data->state) << std::endl;
		SECTION("QFirst") {
            identityMeas<OrientationMeasurementUsingPolicy<kalman::QFirst>>(
                *data, xformedMeas);
        }
#if 1
		SECTION("QLast") {
            identityMeas<OrientationMeasurementUsingPolicy<kalman::QLast>>(
                *data, xformedMeas);
        }
		SECTION("SplitQ") {
            identityMeas<OrientationMeasurementUsingPolicy<kalman::SplitQ>>(
                *data, xformedMeas);
        }
#endif	
    }
}
#if 0
int main() { return osvr::vbtracker::doTests(); }
#endif
