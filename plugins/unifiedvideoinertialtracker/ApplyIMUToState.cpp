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
#include "CrossProductMatrix.h"
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

    inline Eigen::Quaterniond
    computeEffectiveIMUToCam(Eigen::Quaterniond const &cameraPose,
                             util::Angle yawCorrection) {
        return Eigen::Quaterniond(
            Eigen::AngleAxisd(util::getRadians(yawCorrection),
                              Eigen::Vector3d::UnitY()) *
            cameraPose);
    }
    /// The measurement here has been split into a base and derived type, so
    /// that the derived type only contains the little bit that depends on a
    /// particular state type.
    class IMUOrientationMeasBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 3;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementSquareMatrix = types::SquareMatrix<DIMENSION>;
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
/// m_quat is iRbm - the body (measured) frame
/// cRb is "current" (predicted) state - body in camera space
#ifdef OSVR_Q_LAST
            return cRb.inverse() * m_cRi * m_quat;
#else
            // this is the logical one.
            // return m_quat.conjugate() * m_iRc * cRb;
            // this is the one that matches the original measurement class
            // closer.
            return m_cRi * m_quat * cRb.inverse();
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
#ifdef OSVR_FLIP_QUATS
            return residual.squaredNorm() < equivResidual.squaredNorm()
                       ? residual
                       : equivResidual;
#else
            return residual;
#endif
        }
        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(Eigen::Quaterniond const &quat) { m_quat = quat; }

        /// Get the block of jacobian that is non-zero: your subclass will have
        /// to put it where it belongs for each particular state type.
        template <typename State>
        types::Matrix<DIMENSION, 3> getJacobianBlock(State const &s) const {
            /// Compute the innovation quat again.
            Eigen::Quaterniond q = getInnovationQuat(s.getCombinedQuaternion());
#ifdef OSVR_USE_CODEGEN
            /// pre-compute the manually-extracted subexpressions.
            Eigen::Vector3d q2(q.vec().cwiseProduct(q.vec()));
            auto qvecnorm = std::sqrt(q2.sum());
            /// codegen follows
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
            Eigen::Matrix<double, 3, 3> ret;
            ret << q2.x() * tmp3 + tmp1, tmp4 + tmp6, -tmp7 + tmp8,
                -tmp4 + tmp6, q2.y() * tmp3 + tmp1, tmp10 + tmp9, tmp7 + tmp8,
                tmp10 - tmp9, q2.z() * tmp3 + tmp1;
#else
            auto qvecnorm = q.vec().norm();

            if (qvecnorm < 1e-4) {
                // effectively 0 - must avoid divide by zero.
                // All numerators also go to 0 in this case, so we'll just
                // eliminate them.
                // The exception is the second term of the main diagonal:
                // nominally -qw/2*vecnorm.
                // qw would be 1, but in this form at least, the math goes to
                // -infinity.

                // When the jacobian is computed symbolically specifically for
                // the case that q is the identity quaternion, a zero matrix is
                // returned.
                return Eigen::Matrix3d::Zero();
            }
            Eigen::Matrix3d ret =
                // first term: qw * (qvec outer product with itself), over 2 *
                // vecnorm^3
                (q.w() * q.vec() * q.vec().transpose() /
                 (2 * qvecnorm * qvecnorm * qvecnorm)) +
                // second term of each element: cross-product matrix of the
                // negative vector part of quaternion over 2*vecnorm, minus the
                // identity matrix * qw/2*vecnorm
                ((vbtracker::skewSymmetricCrossProductMatrix3(-q.vec()) -
                  Eigen::Matrix3d::Identity() * q.w()) /
                 (2 * qvecnorm));
            static bool printedJacobian = false;
            if (!printedJacobian) {
                printedJacobian = true;
                std::cout << ret << std::endl;
            }
#endif

#ifdef OSVR_Q_LAST
            return ret;
#else
            // Quat.QuatToRotVec(q * g(x))
            // turns out the output is the same as the above, just transposed,
            // even after the conversion to a rotation vector. (given a fixed q)
            return ret.transpose();
#endif
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

        /// Measurement class will rotate it into camera space
        /// @todo do this without rotating into camera space?

        /// @todo transform variance?

        kalman::IMUOrientationMeasurement<BodyState> kalmanMeas{
            getCameraRotation(sys), yawCorrection, quat, var};
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
