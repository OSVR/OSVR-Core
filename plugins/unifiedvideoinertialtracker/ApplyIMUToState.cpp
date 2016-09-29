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

#undef OSVR_USE_OLD_MEASUREMENT_CLASS

namespace osvr {

namespace kalman {
    namespace {
        struct QLast {
            /// The guts of QLast and QFirst's jacobian are the same - they
            /// differ by the innovation quat they take in, and by a transpose
            /// on the output.
            static Eigen::Matrix3d
            getCommonSingleQJacobian(Eigen::Quaterniond const &innovationQuat) {
                auto &q = innovationQuat;
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
                    -tmp4 + tmp6, q2.y() * tmp3 + tmp1, tmp10 + tmp9,
                    tmp7 + tmp8, tmp10 - tmp9, q2.z() * tmp3 + tmp1;
#else
                auto qvecnorm = q.vec().norm();

                if (qvecnorm < 1e-4) {
                    // effectively 0 - must avoid divide by zero.
                    // All numerators also go to 0 in this case, so we'll just
                    // eliminate them.
                    // The exception is the second term of the main diagonal:
                    // nominally -qw/2*vecnorm.
                    // qw would be 1, but in this form at least, the math goes
                    // to
                    // -infinity.

                    // When the jacobian is computed symbolically specifically
                    // for
                    // the case that q is the identity quaternion, a zero matrix
                    // is
                    // returned.
                    return Eigen::Matrix3d::Zero();
                }
                Eigen::Matrix3d ret =
                    // first term: qw * (qvec outer product with itself), over 2
                    // *
                    // vecnorm^3
                    (q.w() * q.vec() * q.vec().transpose() /
                     (2 * qvecnorm * qvecnorm * qvecnorm)) +
                    // second term of each element: cross-product matrix of the
                    // negative vector part of quaternion over 2*vecnorm, minus
                    // the
                    // identity matrix * qw/2*vecnorm
                    ((vbtracker::skewSymmetricCrossProductMatrix3(-q.vec()) -
                      Eigen::Matrix3d::Identity() * q.w()) /
                     (2 * qvecnorm));
#endif
                return ret;
            }
            static Eigen::Quaterniond
            getInnovationQuat(Eigen::Quaterniond const &measInCamSpace,
                              Eigen::Quaterniond const &cRb) {
                return cRb.inverse() * measInCamSpace;
            }
            static Eigen::Matrix3d
            getJacobian(Eigen::Quaterniond const &measInCamSpace,
                        Eigen::Quaterniond const &cRb) {
                Eigen::Quaterniond q = getInnovationQuat(measInCamSpace, cRb);
                return getCommonSingleQJacobian(q);
            }
        };
        struct QFirst {
            static Eigen::Quaterniond
            getInnovationQuat(Eigen::Quaterniond const &measInCamSpace,
                              Eigen::Quaterniond const &cRb) {
                return measInCamSpace.conjugate() * cRb;
            }
            static Eigen::Matrix3d
            getJacobian(Eigen::Quaterniond const &measInCamSpace,
                        Eigen::Quaterniond const &cRb) {
                Eigen::Quaterniond q = getInnovationQuat(measInCamSpace, cRb);
                return QLast::getCommonSingleQJacobian(q).transpose();
            }
        };

        struct SplitQ {
            static Eigen::Quaterniond
            getInnovationQuat(Eigen::Quaterniond const &measInCamSpace,
                              Eigen::Quaterniond const &cRb) {

                // this is the one that matches the original measurement class
                // closest, despite the coordinate systems not making 100%
                // sense.
                return measInCamSpace * cRb.inverse();
            }
            static Eigen::Matrix3d
            getJacobian(Eigen::Quaterniond const &measInCamSpace,
                        Eigen::Quaterniond const &cRb) {

                Eigen::Quaterniond const &q = measInCamSpace;
                Eigen::Quaterniond state_inv = cRb.inverse();

#ifdef OSVR_USE_CODEGEN
                auto tmp0 = (1. / 2.) * state_inv.w();
                auto tmp1 = -q.w() * tmp0;
                auto tmp2 = (1. / 2.) * state_inv.x();
                auto tmp3 = q.x() * tmp2;
                auto tmp4 = (1. / 2.) * state_inv.y();
                auto tmp5 = q.y() * tmp4;
                auto tmp6 = -tmp5;
                auto tmp7 = (1. / 2.) * state_inv.z();
                auto tmp8 = q.z() * tmp7;
                auto tmp9 = -tmp8;
                auto tmp10 = q.z() * tmp0;
                auto tmp11 = q.w() * tmp7;
                auto tmp12 = q.x() * tmp4 + q.y() * tmp2;
                auto tmp13 = q.y() * tmp0;
                auto tmp14 = q.w() * tmp4;
                auto tmp15 = q.x() * tmp7 + q.z() * tmp2;
                auto tmp16 = tmp1 - tmp3;
                auto tmp17 = q.x() * tmp0;
                auto tmp18 = q.w() * tmp2;
                auto tmp19 = q.y() * tmp7 + q.z() * tmp4;
                Eigen::Matrix<double, 3, 3> ret;
                ret << tmp1 + tmp3 + tmp6 + tmp9, -tmp10 + tmp11 + tmp12,
                    tmp13 - tmp14 + tmp15, tmp10 - tmp11 + tmp12,
                    tmp16 + tmp5 + tmp9, -tmp17 + tmp18 + tmp19,
                    -tmp13 + tmp14 + tmp15, tmp17 - tmp18 + tmp19,
                    tmp16 + tmp6 + tmp8;
                return ret.transpose();
#else
                auto qvecnorm2 = q.vec().squaredNorm();

                if (qvecnorm2 < 1e-4) {
                    // effectively 0 - so identity quat.
                    // Can simplify here.

                    Eigen::Matrix3d ret;
                    ret << -state_inv.w(), state_inv.z(),
                        -state_inv.y(),                                //
                        -state_inv.z(), -state_inv.w(), state_inv.x(), //
                        state_inv.y(), -state_inv.x(), -state_inv.w();
                    return ret / 2.;
                }
                Eigen::Matrix<double, 3, 4> A;
                A << -q.w(), q.x(), -q.y(),
                    -q.z(), //
                    q.z(), q.y(), q.x(),
                    -q.w(), // row 2
                    -q.y(), q.z(), q.w(), q.x();
                Eigen::Matrix<double, 4, 3> B;
                B << state_inv.w(), -state_inv.z(),
                    state_inv.y(), //
                    state_inv.x(), state_inv.y(),
                    state_inv.z(), //
                    state_inv.y(), -state_inv.x(),
                    -state_inv.w(), //
                    state_inv.z(), state_inv.w(), -state_inv.x();
                Eigen::Matrix3d ret = A * B / 2.;
#if 0
				/// rotate a quarter turn ccw
				ret << initialret.rightCols<1>().transpose(),
					initialret.col(1).transpose(),
					initialret.leftCols<1>().transpose();
#endif
                return ret;
#endif // OSVR_USE_CODEGEN
            }
        };
    } // namespace

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
        /// Quat should already by in camera space!
        IMUOrientationMeasBase(Eigen::Quaterniond const &quat,
                               types::Vector<3> const &emVariance)
            : m_quat(quat), m_covariance(emVariance.asDiagonal()) {}

        template <typename State>
        MeasurementSquareMatrix const &getCovariance(State const &) {
            return m_covariance;
        }

        /// Specifies which innovation computation and jacobian to use.
        using Policy = SplitQ;
        // using Policy = QLast;
        // using Policy = QFirst;

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can
        /// `.getCombinedQuaternion()`
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            const Eigen::Quaterniond residualq =
                Policy::getInnovationQuat(m_quat, s.getCombinedQuaternion());

            // Two equivalent quaternions: but their logs are typically
            // different: one is the "short way" and the other is the "long
            // way". We'll compute both and pick the "short way".
            MeasurementVector residual = util::quat_ln(residualq);
            MeasurementVector equivResidual =
                util::quat_ln(Eigen::Quaterniond(-(residualq.coeffs())));
#if 1
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
            return Policy::getJacobian(m_quat, s.getCombinedQuaternion());
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

        /// Quat should already be rotated into camera space.
        IMUOrientationMeasurement(Eigen::Quaterniond const &quat,
                                  types::Vector<3> const &emVariance)
            : Base(quat, emVariance) {}

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

#ifdef OSVR_USE_OLD_MEASUREMENT_CLASS

        kalman::AbsoluteOrientationMeasurement<BodyState> kalmanMeas(
            getCameraRotation(sys).inverse() * quat, var);
#else
        Eigen::Quaterniond measInCamSpace =
            Eigen::Quaterniond(sys.getRoomToCamera().rotation()) * quat;
        kalman::IMUOrientationMeasurement<BodyState> kalmanMeas{measInCamSpace,
                                                                var};
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
#if 0
        static ::util::Stride s(401);
        if (++s) {

            std::cout << "delta z\t "
                      << correctionInProgress.deltaz.transpose();
            std::cout << "\t state correction "
                      << correctionInProgress.stateCorrection.transpose()
                      << "\n";
        }
#endif
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
