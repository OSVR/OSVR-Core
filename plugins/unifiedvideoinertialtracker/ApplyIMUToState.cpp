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
#include "AngVelTools.h"
#include "SpaceTransformations.h"

// Library/third-party includes
#include <osvr/Kalman/AbsoluteOrientationMeasurement.h>
#include <osvr/Kalman/AngularVelocityMeasurement.h>
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <osvr/Util/EigenExtras.h>
#include <osvr/Util/EigenQuatExponentialMap.h>
#include <util/Stride.h>

// Standard includes
#include <iostream>

namespace osvr {

namespace kalman {
#undef OSVR_Q_LAST
    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class IMUOrientationMeasBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementSquareMatrix = types::SquareMatrix<DIMENSION>;
        static Eigen::Quaterniond
        computeEffectiveIMUToCam(Eigen::Quaterniond const &cameraPose,
                                 util::Angle yawCorrection) {
            return Eigen::Quaterniond(
                Eigen::AngleAxisd(util::getRadians(yawCorrection),
                                  Eigen::Vector3d::UnitY()) *
                cameraPose);
        }
        IMUOrientationMeasBase(Eigen::Quaterniond const &cameraPose,
                               util::Angle yawCorrection,
                               Eigen::Quaterniond const &quat,
                               types::Vector<3> const &emVariance)
            : m_iRc(computeEffectiveIMUToCam(cameraPose, yawCorrection)),
              m_cRi(m_iRc.inverse()), m_quat(quat),
              m_covariance(emVariance.asDiagonal()) {}

        template <typename State>
        MeasurementSquareMatrix const &getCovariance(State const &) {
            return m_covariance;
        }

        /// convenience function for shared implementation between getResidual
        /// and getJacobianBlock
        Eigen::Quaterniond
        getInnovationQuat(Eigen::Quaterniond const &cRb) const {
/// m_quat is iRbm - the body (modified) frame

#ifdef OSVR_Q_LAST
            return (m_iRc * cRb).conjugate() * m_quat;
#else
            return util::flipQuatSignToMatch(cRb, m_quat.conjugate() * m_iRc) *
                   cRb;
#endif
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can
        /// `.getCombinedQuaternion()`
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
#if 0
            const Eigen::Quaterniond predictionInIMUSpace =
                m_iRc * s.getCombinedQuaternion();

            const Eigen::Quaterniond residualqInIMUSpace = m_quat * predictionInIMUSpace.inverse();
			const Eigen::Quaterniond residualqInCamSpace = m_cRi * residualqInIMUSpace;
#else

            const Eigen::Quaterniond residualq =
                getInnovationQuat(s.getCombinedQuaternion());
#endif
            // Two equivalent quaternions: but their logs are typically
            // different: one is the "short way" and the other is the "long
            // way". We'll compute both and pick the "short way".
            MeasurementVector residual = util::quat_ln(residualq);
            MeasurementVector equivResidual =
                util::quat_ln(Eigen::Quaterniond(-(residualq.coeffs())));
            return residual.squaredNorm() < equivResidual.squaredNorm()
                       ? residual
                       : equivResidual;
        }
        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(Eigen::Quaterniond const &quat) { m_quat = quat; }

        /// Get the block of jacobian that is non-zero: your subclass will have
        /// to put it where it belongs for each particular state type.
        template <typename State>
        types::Matrix<DIMENSION, 3> getJacobianBlock(State const &s) const {
            /// Compute the innovation quat again.
            Eigen::Quaterniond q = getInnovationQuat(s.getCombinedQuaternion());
            /// pre-compute the manually-extracted subexpressions.
            Eigen::Vector3d q2(q.vec().cwiseProduct(q.vec()));
            auto qvecnorm = std::sqrt(q2.sum());
/// codegen follows
#ifdef OSVR_Q_LAST
            // Quat.QuatToRotVec(g(x) * q)
            auto tmp0 = (1. / 2.) / qvecnorm;
            auto tmp1 = -q.w() * tmp0;
            auto tmp2 = std::pow(qvecnorm, -3);
            auto tmp3 = (1. / 2.) * q.w() * tmp2;
            auto tmp4 = q.z() * tmp0;
            auto tmp5 = (1. / 2.) * q.w() * q.x() * tmp2;
            auto tmp6 = q.y() * tmp5;
            auto tmp7 = q.y() * tmp0;
            auto tmp8 = q.z() * tmp5;
            auto tmp9 = q.x() * tmp0;
            auto tmp10 = q.y() * q.z() * tmp3;
            Eigen::Matrix3d ret;
            ret << q2.x() * tmp3 + tmp1, -tmp4 + tmp6, tmp7 + tmp8, tmp4 + tmp6,
                q2.y() * tmp3 + tmp1, tmp10 - tmp9, -tmp7 + tmp8, tmp10 + tmp9,
                q2.z() * tmp3 + tmp1;
#else
            // Quat.QuatToRotVec(q * g(x))
            auto tmp0 = (1. / 2.) / qvecnorm;
            auto tmp1 = -q.w() * tmp0;
            auto tmp2 = std::pow(qvecnorm, -3);
            auto tmp3 = (1. / 2.) * q.w() * tmp2;
            auto tmp4 = q.z() * tmp0;
            auto tmp5 = (1. / 2.) * q.w() * q.x() * tmp2;
            auto tmp6 = q.y() * tmp5;
            auto tmp7 = q.y() * tmp0;
            auto tmp8 = q.z() * tmp5;
            auto tmp9 = q.x() * tmp0;
            auto tmp10 = q.y() * q.z() * tmp3;
            Eigen::Matrix3d ret;
            ret << q2.x() * tmp3 + tmp1, tmp4 + tmp6, -tmp7 + tmp8,
                -tmp4 + tmp6, q2.y() * tmp3 + tmp1, tmp10 + tmp9, tmp7 + tmp8,
                tmp10 - tmp9, q2.z() * tmp3 + tmp1;
#endif
            static bool printedJacobian = false;
            if (!printedJacobian) {
                printedJacobian = true;
                std::cout << ret << std::endl;
            }
            return ret;
        }

      private:
        Eigen::Quaterniond m_iRc;
        Eigen::Quaterniond m_cRi;
        Eigen::Quaterniond m_quat;
        MeasurementSquareMatrix m_covariance;
    };

    /// This is the subclass of AbsoluteOrientationBase: only explicit
    /// specializations, and on state types.
    template <typename StateType> class IMUOrientationMeasurement;

    /// AbsoluteOrientationMeasurement with a pose_externalized_rotation::State
    template <>
    class IMUOrientationMeasurement<pose_externalized_rotation::State>
        : public IMUOrientationMeasBase {
      public:
        using State = pose_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = IMUOrientationMeasBase;

        IMUOrientationMeasurement(Eigen::Quaterniond const &cameraPose,
                                  util::Angle yawCorrection,
                                  Eigen::Quaterniond const &quat,
                                  types::Vector<3> const &emVariance)
            : Base(cameraPose, yawCorrection, quat, emVariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &s) const {
            using namespace pose_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<DIMENSION, 3>(0, 3) = Base::getJacobianBlock(s);
            return ret;
        }
    };
} // namespace kalman
namespace vbtracker {
    inline void applyOriToState(TrackingSystem const &sys, BodyState &state,
                                BodyProcessModel &processModel,
                                CannedIMUMeasurement const &meas) {
        Eigen::Quaterniond quat;
        meas.restoreQuat(quat);
        Eigen::Vector3d var;
        meas.restoreQuatVariance(var);
        util::Angle yawCorrection = meas.getYawCorrection();

        /// Rotate it into camera space
        /// @todo do this without rotating into camera space?
        // quat = getQuatToCameraSpace(sys) * quat;
        /// @todo transform variance?

        kalman::IMUOrientationMeasurement<BodyState> kalmanMeas{
            Eigen::Quaterniond(sys.getCameraPose().rotation()), yawCorrection,
            quat, var};
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
    }

    inline void applyAngVelToState(TrackingSystem const &sys, BodyState &state,
                                   BodyProcessModel &processModel,
                                   CannedIMUMeasurement const &meas) {

        Eigen::Vector3d angVel;
        meas.restoreAngVel(angVel);
        Eigen::Vector3d var;
        meas.restoreAngVelVariance(var);
        static const double dt = 0.02;
        /// Rotate it into camera space - it's bTb and we want cTc
        /// @todo do this without rotating into camera space?
        Eigen::Quaterniond cTb = state.getQuaternion();
        // Eigen::Matrix3d bTc(state.getQuaternion().conjugate());
        /// Turn it into an incremental quat to do the space transformation
        Eigen::Quaterniond incrementalQuat =
            cTb * angVelVecToIncRot(angVel, dt) * cTb.conjugate();
        /// Then turn it back into an angular velocity vector
        angVel = incRotToAngVelVec(incrementalQuat, dt);
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
