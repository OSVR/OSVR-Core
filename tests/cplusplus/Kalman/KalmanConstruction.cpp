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
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/PoseConstantVelocity.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <iostream>

using ProcessModel = osvr::kalman::PoseConstantVelocityProcessModel;
using State = ProcessModel::State;
using Filter = osvr::kalman::FlexibleKalmanFilter<ProcessModel>;

TEST(KalmanFilter, DefaultConstruction) {
    ASSERT_NO_THROW(auto filter = Filter{});
}
TEST(KalmanFilter, MoveStateConstructions) {
    ASSERT_NO_THROW(auto filter = Filter{State{}});
}
TEST(KalmanFilter, MoveStateProcessConstructions) {
    ASSERT_NO_THROW(auto filter = Filter(ProcessModel{}, State{}));
}
