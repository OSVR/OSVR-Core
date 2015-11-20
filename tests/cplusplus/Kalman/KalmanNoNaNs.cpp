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

#include <iostream>
#include <osvr/Util/EigenCoreGeometry.h>
template <typename T>
inline void dumpKalmanDebugOuput(const char name[], const char expr[],
                                 T const &value) {
    std::cout << "\n(Kalman Debug Output) " << name << " [" << expr << "]:\n"
              << value << std::endl;
}

#define OSVR_KALMAN_DEBUG_OUTPUT(Name, Value)                                  \
    dumpKalmanDebugOuput(Name, #Value, Value)

// Internal Includes
#include "ContentsInvalid.h"
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Kalman/PoseConstantVelocity.h>
#include <osvr/Kalman/PoseDampedConstantVelocity.h>
#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AbsolutePositionMeasurement.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
#include <iostream>

using ProcessModel = osvr::kalman::PoseConstantVelocityProcessModel;
using State = ProcessModel::State;
using AbsoluteOrientationMeasurement =
    osvr::kalman::AbsoluteOrientationMeasurement<State>;
using AbsolutePositionMeasurement =
    osvr::kalman::AbsolutePositionMeasurement<State>;
using Filter = osvr::kalman::FlexibleKalmanFilter<ProcessModel>;

class Stability : public ::testing::Test {
  public:
    template <typename Filter> void dumpInitialState(Filter const &filter) {
        ASSERT_EQ(iteration, 0)
            << "Internal test logic error: should only call "
               "dumpInitialState before iterations begin.";
        dumpState(filter.state(), "Initial state");
    }

    template <typename State>
    void dumpState(State const &state, const char msg[]) {
        std::cout << "\n" << msg << " (iteration " << iteration << "):\n"
                  << state << std::endl;
    }
    template <typename Filter, typename Measurement>
    void filterAndCheck(Filter &filter, Measurement &meas, double dt = 0.1) {
        filter.predict(dt);
        dumpState(filter.state(), "After prediction");
        ASSERT_FALSE(stateContentsInvalid(filter.state()))
            << "ERROR: Detected an invalid floating-point value in the state "
               "after prediction step of iteration "
            << iteration;
        ASSERT_FALSE(covarianceContentsInvalid(filter.state()))
            << "ERROR: Detected an invalid state covariance matrix (invalid "
               "floating-point, or invariants violated) after prediction step "
               "of iteration "
            << iteration;

        ASSERT_FALSE(
            covarianceContentsInvalid(meas.getCovariance(filter.state())))
            << "ERROR: Detected invalid contents (invalid floating-point, or "
               "invariants violated) in measurement covariance "
               "matrix, iteration "
            << iteration << "\n" << meas.getCovariance(filter.state());

        filter.correct(meas);
        dumpState(filter.state(), "After correction");
        ASSERT_FALSE(stateContentsInvalid(filter.state()))
            << "ERROR: Detected an invalid floating-point value in the state "
               "after correction step of iteration "
            << iteration;
        ASSERT_FALSE(covarianceContentsInvalid(filter.state()))
            << "ERROR: Detected an invalid state covariance matrix (invalid "
               "floating-point, or invariants violated) after correction step "
               "of iteration "
            << iteration;
        iteration++;
    }

    template <typename Filter, typename Measurement>
    void filterAndCheckRepeatedly(Filter &filter, Measurement &meas,
                                  double dt = 0.1,
                                  std::size_t iterations = 100) {
        for (iteration = 0; iteration < iterations; iteration++) {
            filter.predict(dt);
            dumpState(filter.state(), "After prediction");
            ASSERT_FALSE(stateContentsInvalid(filter.state()))
                << "ERROR: Detected an invalid floating-point value in the "
                   "state "
                   "after prediction step of iteration "
                << iteration;
            ASSERT_FALSE(covarianceContentsInvalid(filter.state()))
                << "ERROR: Detected an invalid state covariance matrix "
                   "(invalid "
                   "floating-point, or invariants violated) after prediction "
                   "step "
                   "of iteration "
                << iteration;

            ASSERT_FALSE(
                covarianceContentsInvalid(meas.getCovariance(filter.state())))
                << "ERROR: Detected invalid contents (invalid floating-point, "
                   "or "
                   "invariants violated) in measurement covariance "
                   "matrix, iteration "
                << iteration << "\n" << meas.getCovariance(filter.state());

            filter.correct(meas);
            dumpState(filter.state(), "After correction");
            ASSERT_FALSE(stateContentsInvalid(filter.state()))
                << "ERROR: Detected an invalid floating-point value in the "
                   "state "
                   "after correction step of iteration "
                << iteration;
            ASSERT_FALSE(covarianceContentsInvalid(filter.state()))
                << "ERROR: Detected an invalid state covariance matrix "
                   "(invalid "
                   "floating-point, or invariants violated) after correction "
                   "step "
                   "of iteration "
                << iteration;
        }
    }

  private:
    std::size_t iteration = 0;
};

template <typename T> class VariedProcessModelStability : public Stability {};

typedef ::testing::Types<osvr::kalman::PoseConstantVelocityProcessModel,
                         osvr::kalman::PoseDampedConstantVelocityProcessModel>
    ProcessModelTypes;

TYPED_TEST_CASE(VariedProcessModelStability, ProcessModelTypes);
TYPED_TEST(VariedProcessModelStability,
           IdentityAbsoluteOrientationMeasurement) {
    using Filter = osvr::kalman::FlexibleKalmanFilter<TypeParam>;

    auto filter = Filter{};
    auto meas = AbsoluteOrientationMeasurement{
        Eigen::Quaterniond::Identity(),
        Eigen::Vector3d(0.00001, 0.00001, 0.00001)};
    this->dumpInitialState(filter);
    this->filterAndCheckRepeatedly(filter, meas);
    /// @todo check that it's roughly identity
}

TYPED_TEST(VariedProcessModelStability, IdentityAbsolutePositionMeasurement) {
    using Filter = osvr::kalman::FlexibleKalmanFilter<TypeParam>;

    auto filter = Filter{};
    auto meas = AbsolutePositionMeasurement{
        Eigen::Vector3d::Zero(), Eigen::Vector3d::Constant(0.000007)};
    this->dumpInitialState(filter);
    this->filterAndCheckRepeatedly(filter, meas);
    /// @todo check that it's roughly identity
}

TYPED_TEST(VariedProcessModelStability, AbsolutePositionMeasurementXlate111) {
    using Filter = osvr::kalman::FlexibleKalmanFilter<TypeParam>;

    auto filter = Filter{};
    auto meas = AbsolutePositionMeasurement{
        Eigen::Vector3d::Constant(1), Eigen::Vector3d::Constant(0.000007)};
    this->dumpInitialState(filter);
    this->filterAndCheckRepeatedly(filter, meas);
    /// @todo check that it's roughly identity orientation, position of 1, 1, 1
}
