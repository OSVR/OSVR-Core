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
#include "ApplyIMUToState.h"
#include "KalmanCorrect.h"
#include "SpaceTransformations.h"

// Library/third-party includes
//#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AngularVelocityMeasurement.h>
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Util/EigenQuatExponentialMap.h>
#include <util/Stride.h>

// Standard includes
#include <iostream>


namespace osvr {
namespace kalman {
    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class AbsoluteOrientationBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementSquareMatrix = types::SquareMatrix<DIMENSION>;
        AbsoluteOrientationBase(Eigen::Quaterniond const &quat,
                                types::Vector<3> const &emVariance)
            : m_quat(quat), m_covariance(emVariance.asDiagonal()) {}

        template <typename State>
        MeasurementSquareMatrix const &getCovariance(State const &) {
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can
        /// `.getCombinedQuaternion()`
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            const Eigen::Quaterniond prediction = s.getCombinedQuaternion();
            const Eigen::Quaterniond residual = m_quat * prediction.inverse();
            // Use the dot product to choose which of the two equivalent
            // quaternions to get the log of for the residual.
            const Eigen::Quaterniond equivalentResidual =
                Eigen::Quaterniond(-(residual.coeffs()));
            auto dot = prediction.dot(residual);
#if 0
            return dot >= 0 ? util::quat_exp_map(residual).ln()
                            : util::quat_exp_map(equivalentResidual).ln();
#else
            return dot < 0 ? util::quat_exp_map(residual).ln()
                            : util::quat_exp_map(equivalentResidual).ln();

#endif
        }
        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(Eigen::Quaterniond const &quat) { m_quat = quat; }

        /// Get the block of jacobian that is non-zero: your subclass will have
        /// to put it where it belongs for each particular state type.
        types::Matrix<DIMENSION, 3> getJacobianBlock() const {
            return Eigen::Matrix3d::Identity();
        }

      private:
        Eigen::Quaterniond m_quat;
        MeasurementSquareMatrix m_covariance;
    };

    /// This is the subclass of AbsoluteOrientationBase: only explicit
    /// specializations, and on state types.
    template <typename StateType> class AbsoluteOrientationMeasurement;

    /// AbsoluteOrientationMeasurement with a pose_externalized_rotation::State
    template <>
    class AbsoluteOrientationMeasurement<pose_externalized_rotation::State>
        : public AbsoluteOrientationBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = AbsoluteOrientationBase;

        AbsoluteOrientationMeasurement(Eigen::Quaterniond const &quat,
                                       types::Vector<3> const &eulerVariance)
            : Base(quat, eulerVariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &s) const {
            using namespace pose_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<DIMENSION, 3>(0, 3) = Base::getJacobianBlock();
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr

namespace osvr {
namespace vbtracker {
    inline void applyOriToState(TrackingSystem const &sys, BodyState &state,
                                BodyProcessModel &processModel,
                                CannedIMUMeasurement const &meas) {
        Eigen::Quaterniond quat;
        meas.restoreQuat(quat);
        Eigen::Vector3d var;
        meas.restoreQuatVariance(var);

        /// Rotate it into camera space
        /// @todo do this without rotating into camera space?
        quat = getQuatToCameraSpace(sys) * quat;
        /// @todo transform variance?

        kalman::AbsoluteOrientationMeasurement<BodyState> kalmanMeas{quat, var};
#if 0
        {
            static ::util::Stride s(201);
            if (++s) {
                Eigen::AngleAxisd pred(state.getCombinedQuaternion());
                Eigen::AngleAxisd meas(quat);
                std::cout << "Predicted: " << pred.angle() << " about "
                          << pred.axis().transpose() << "\n";
                std::cout << "Measured: " << meas.angle() << " about "
                          << meas.axis().transpose() << "\n";
                std::cout << "Residual: "
                          << kalmanMeas.getResidual(state).transpose() << "\n";
            }
        }
#endif
        auto correctionInProgress =
            kalman::beginCorrection(state, processModel, kalmanMeas);
        auto outputMeas = [&] {
            std::cout << "state: " << state.getQuaternion().coeffs().transpose()
                      << " and measurement: " << quat.coeffs().transpose();
        };
        if (!correctionInProgress.stateCorrectionFinite) {
            std::cout
                << "Non-finite state correction in applying orientation: ";
            outputMeas();
            std::cout << "\n";
            return;
        }
        if (!correctionInProgress.finishCorrection(true)) {
            std::cout
                << "Non-finite error covariance after applying orientation: ";
            outputMeas();
            std::cout << "\n";
        }
        // kalman::correct(state, processModel, kalmanMeas);
    }

    inline void applyAngVelToState(TrackingSystem const &sys, BodyState &state,
                                   BodyProcessModel &processModel,
                                   CannedIMUMeasurement const &meas) {

        Eigen::Vector3d angVel;
        meas.restoreAngVel(angVel);
        Eigen::Vector3d var;
        meas.restoreAngVelVariance(var);

        /// Rotate it into camera space - it's bTb and we want cTc
        /// @todo do this without rotating into camera space?
        Eigen::Quaterniond cTb = state.getQuaternion();
        // Eigen::Matrix3d bTc(state.getQuaternion().conjugate());
        Eigen::Quaterniond incrementalQuat =
            cTb * util::quat_exp_map(angVel).exp() * cTb.conjugate();
        angVel = util::quat_exp_map(incrementalQuat).ln();
        // angVel = (getRotationMatrixToCameraSpace(sys) * angVel).eval();
        /// @todo transform variance?

        kalman::AngularVelocityMeasurement<BodyState> kalmanMeas{angVel, var};
        kalman::correct(state, processModel, kalmanMeas);
    }

    void applyIMUToState(TrackingSystem const &sys,
                         util::time::TimeValue const &initialTime,
                         BodyState &state, BodyProcessModel &processModel,
                         util::time::TimeValue const &newTime,
                         CannedIMUMeasurement const &meas) {
        if (newTime != initialTime) {
            auto dt = osvrTimeValueDurationSeconds(&newTime, &initialTime);
            kalman::predict(state, processModel, dt);
            state.externalizeRotation();
        }
        if (meas.orientationValid()) {
            applyOriToState(sys, state, processModel, meas);
        } else if (meas.angVelValid()) {
            applyAngVelToState(sys, state, processModel, meas);

        } else {
            // unusually, the measurement is totally invalid. Just normalize and
            // go on.
            state.postCorrect();
        }
    }
} // namespace vbtracker
} // namespace osvr
