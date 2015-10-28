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
#include "PoseDampedConstantVelocity.h"
#include "AbsoluteOrientationMeasurement.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

int main() {
    using namespace osvr::kalman;
    using ProcessModel = osvr::kalman::PoseDampedConstantVelocityProcessModel;
    using State = ProcessModel::State;
    using Measurement = osvr::kalman::AbsoluteOrientationMeasurement<State>;
    using Filter = osvr::kalman::FlexibleKalmanFilter<ProcessModel>;
    auto filter = Filter{ProcessModel{}, State{}};
    {
        auto meas = Measurement{Eigen::Quaterniond::Identity(),
                                Eigen::Vector3d(0.00001, 0.00001, 0.00001)};
        std::cout << filter.state() << std::endl;
        filter.predict(0.1);
        std::cout << filter.state() << std::endl;
        filter.correct(meas);
        std::cout << filter.state() << std::endl;
    }

    {
        auto defaultFilter = Filter{};
        auto moveStateFilter = Filter{State{}};
    }
    return 0;
}
