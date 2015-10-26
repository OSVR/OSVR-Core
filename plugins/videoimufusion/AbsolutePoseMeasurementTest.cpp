/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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
#include "FlexibleKalmanFilter.h"
#include "PoseState.h"
#include "PoseDampedConstantVelocity.h"
#include "AbsolutePoseMeasurement.h"

// Library/third-party includes
// - none

// Standard includes
// - none

int main() {
    using namespace osvr::kalman;
    using State = osvr::kalman::pose_externalized_rotation::State;
    using ProcessModel = osvr::kalman::PoseDampedConstantVelocityProcessModel;
    using Measurement = osvr::kalman::AbsolutePoseMeasurement<State>;
    using Filter = osvr::kalman::FlexibleKalmanFilter<State, ProcessModel>;
    auto filter = Filter{State{}, ProcessModel{}};
    {
        auto meas =
            Measurement{Eigen::Vector3d::Zero(), Eigen::Quaterniond::Identity(),
                        Eigen::Matrix<double, 7, 7>::Identity()};
        filter.predict(0.1);
        filter.correct(meas);
    }
    return 0;
}
