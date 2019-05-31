/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#define CATCH_CONFIG_MAIN

// Internal Includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/PoseConstantVelocity.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <iostream>

using ProcessModel = osvr::kalman::PoseConstantVelocityProcessModel;
using State = ProcessModel::State;
using Filter = osvr::kalman::FlexibleKalmanFilter<ProcessModel>;

TEST_CASE("KalmanFilterConstruction") {
    SECTION("default construction") { REQUIRE_NOTHROW(Filter{}); }
    SECTION("move state construction") { REQUIRE_NOTHROW(Filter{State{}}); }

    SECTION("move state+process construction") {
        REQUIRE_NOTHROW(Filter(ProcessModel{}, State{}));
    }
}
