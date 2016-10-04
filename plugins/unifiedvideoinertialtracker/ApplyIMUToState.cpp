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
#define OSVR_USE_CODEGEN

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
                auto q_vecnorm = std::sqrt(q2.sum());
                if (q_vecnorm < 1e-4) {
                    // effectively 0 - must avoid divide by zero.
                    // All numerators also go to 0 in this case, so we'll just
                    // eliminate them.
                    // The exception is the second term of the main diagonal:
                    // nominally -qw/2*vecnorm.
                    // qw would be 1, but in this form at least, the math goes
                    // to -infinity.

                    // When the jacobian is computed symbolically specifically
                    // for the case that q is the identity quaternion, a zero matrix
                    // is returned.
                    return Eigen::Matrix3d::Zero();
                }
                /// codegen follows
                // Quat.QuatToRotVec(g(x) * q)
                auto tmp0 = 1.0 / q_vecnorm;
                auto tmp1 = -q.w() * tmp0;
                auto tmp2 = std::pow(q_vecnorm, -3);
                auto tmp3 = q.w() * tmp2;
                auto tmp4 = q.z() * tmp0;
                auto tmp5 = q.w() * q.x() * tmp2;
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
                Eigen::Quaterniond q2(q.coeffs().cwiseProduct(q.coeffs()));
                auto tmp0 = std::pow(state_inv.x(), 2);
                auto tmp1 = std::pow(state_inv.y(), 2);
                auto tmp2 = std::pow(state_inv.z(), 2);
                auto tmp3 = std::pow(state_inv.w(), 2);
                auto tmp4 = 2 * state_inv.x();
                auto tmp5 = q.w() * q.x() * state_inv.w();
                auto tmp6 = tmp4 * tmp5;
                auto tmp7 = 2 * state_inv.y();
                auto tmp8 = q.w() * q.y() * state_inv.w();
                auto tmp9 = tmp7 * tmp8;
                auto tmp10 = q.w() * q.z() * state_inv.w();
                auto tmp11 = 2 * state_inv.z() * tmp10;
                auto tmp12 = q.x() * q.y() * state_inv.y() * tmp4;
                auto tmp13 = q.x() * q.z() * state_inv.z() * tmp4;
                auto tmp14 = q.y() * q.z() * state_inv.z() * tmp7;
                auto tmp15 = q2.w() * tmp0 + q2.w() * tmp1 + q2.w() * tmp2 +
                             q2.x() * tmp1 + q2.x() * tmp2 + q2.x() * tmp3 +
                             q2.y() * tmp0 + q2.y() * tmp2 + q2.y() * tmp3 +
                             q2.z() * tmp0 + q2.z() * tmp1 + q2.z() * tmp3 +
                             tmp11 - tmp12 - tmp13 - tmp14 + tmp6 + tmp9;
                auto tmp16 = std::abs(tmp15);
                auto tmp17 = std::pow(tmp16, -1. / 2.);
                auto tmp18 =
                    std::sqrt(q2.w() * tmp3 + q2.x() * tmp0 + q2.y() * tmp1 +
                              q2.z() * tmp2 - tmp11 + tmp12 + tmp13 + tmp14 +
                              tmp16 - tmp6 - tmp9);
                auto tmp19 = q.w() * tmp17 * tmp18;
                auto tmp20 = -state_inv.w() * tmp19;
                auto tmp21 = q.z() * tmp17 * tmp18;
                auto tmp22 = state_inv.z() * tmp21;
                auto tmp23 = tmp20 - tmp22;
                auto tmp24 = q.x() * tmp17 * tmp18;
                auto tmp25 = state_inv.x() * tmp24;
                auto tmp26 = q.y() * tmp17 * tmp18;
                auto tmp27 = state_inv.y() * tmp26;
                auto tmp28 = -tmp27;
                auto tmp29 = q2.x() * state_inv.x();
                auto tmp30 = state_inv.w() * tmp29;
                auto tmp31 = q2.y() * state_inv.y();
                auto tmp32 = state_inv.z() * tmp31;
                auto tmp33 = q2.w() * state_inv.w();
                auto tmp34 = state_inv.x() * tmp33;
                auto tmp35 = q2.z() * state_inv.z();
                auto tmp36 = state_inv.y() * tmp35;
                auto tmp37 = q.w() * q.y() * state_inv.y();
                auto tmp38 = state_inv.x() * tmp37;
                auto tmp39 = state_inv.y() * tmp10;
                auto tmp40 = q.w() * q.z() * state_inv.z();
                auto tmp41 = state_inv.x() * tmp40;
                auto tmp42 = q.x() * q.y() * state_inv.w();
                auto tmp43 = state_inv.y() * tmp42;
                auto tmp44 = q.x() * q.y() * state_inv.z();
                auto tmp45 = state_inv.x() * tmp44;
                auto tmp46 = q.x() * q.z() * state_inv.w();
                auto tmp47 = state_inv.z() * tmp46;
                auto tmp48 = q.w() * q.x();
                auto tmp49 = tmp0 * tmp48;
                auto tmp50 = q.y() * q.z();
                auto tmp51 = tmp2 * tmp50;
                auto tmp52 = state_inv.z() * tmp8;
                auto tmp53 = q.x() * q.z() * state_inv.y();
                auto tmp54 = state_inv.x() * tmp53;
                auto tmp55 = tmp3 * tmp48;
                auto tmp56 = tmp1 * tmp50;
                auto tmp57 = tmp30 + tmp32 - tmp34 - tmp36 + tmp38 + tmp39 +
                             tmp41 + tmp43 + tmp45 + tmp47 + tmp49 + tmp51 -
                             tmp52 - tmp54 - tmp55 - tmp56;
                auto tmp58 = (((tmp15) > 0) - ((tmp15) < 0));
                auto tmp59 = std::pow(tmp16, -3. / 2.);
                auto tmp60 = q.y() * state_inv.z() * tmp18 * tmp58 * tmp59;
                auto tmp61 = q.w() * state_inv.x() * tmp18 * tmp58 * tmp59;
                auto tmp62 = q.x() * state_inv.w() * tmp18 * tmp58 * tmp59;
                auto tmp63 = q.z() * state_inv.y() * tmp18 * tmp58 * tmp59;
                auto tmp64 = 1.0 / tmp18;
                auto tmp65 = 2 * q.w() * state_inv.x() * tmp17 * tmp64;
                auto tmp66 = (1. / 2.) * tmp58;
                auto tmp67 =
                    -1. / 2. * tmp30 - 1. / 2. * tmp32 + (1. / 2.) * tmp34 +
                    (1. / 2.) * tmp36 - 1. / 2. * tmp38 - 1. / 2. * tmp39 -
                    1. / 2. * tmp41 - 1. / 2. * tmp43 - 1. / 2. * tmp45 -
                    1. / 2. * tmp47 - 1. / 2. * tmp49 - 1. / 2. * tmp51 +
                    (1. / 2.) * tmp52 + (1. / 2.) * tmp54 + (1. / 2.) * tmp55 +
                    (1. / 2.) * tmp56 + tmp57 * tmp66;
                auto tmp68 = 2 * q.x() * state_inv.w() * tmp17 * tmp64;
                auto tmp69 = 2 * q.y() * state_inv.z() * tmp17 * tmp64;
                auto tmp70 = 2 * q.z() * state_inv.y() * tmp17 * tmp64;
                auto tmp71 = state_inv.x() * tmp26 + state_inv.y() * tmp24;
                auto tmp72 = state_inv.z() * tmp19;
                auto tmp73 = state_inv.w() * tmp21;
                auto tmp74 = state_inv.w() * tmp31;
                auto tmp75 = state_inv.x() * tmp35;
                auto tmp76 = state_inv.y() * tmp33;
                auto tmp77 = state_inv.z() * tmp29;
                auto tmp78 = state_inv.z() * tmp5;
                auto tmp79 = q.w() * q.x() * state_inv.x();
                auto tmp80 = state_inv.y() * tmp79;
                auto tmp81 = state_inv.y() * tmp40;
                auto tmp82 = state_inv.x() * tmp42;
                auto tmp83 = q.y() * q.z() * state_inv.w();
                auto tmp84 = state_inv.z() * tmp83;
                auto tmp85 = q.y() * q.z() * state_inv.x();
                auto tmp86 = state_inv.y() * tmp85;
                auto tmp87 = q.w() * q.y();
                auto tmp88 = tmp1 * tmp87;
                auto tmp89 = q.x() * q.z();
                auto tmp90 = tmp0 * tmp89;
                auto tmp91 = state_inv.x() * tmp10;
                auto tmp92 = state_inv.y() * tmp44;
                auto tmp93 = tmp3 * tmp87;
                auto tmp94 = tmp2 * tmp89;
                auto tmp95 = tmp74 + tmp75 - tmp76 - tmp77 + tmp78 + tmp80 +
                             tmp81 + tmp82 + tmp84 + tmp86 + tmp88 + tmp90 -
                             tmp91 - tmp92 - tmp93 - tmp94;
                auto tmp96 =
                    tmp66 * tmp95 - 1. / 2. * tmp74 - 1. / 2. * tmp75 +
                    (1. / 2.) * tmp76 + (1. / 2.) * tmp77 - 1. / 2. * tmp78 -
                    1. / 2. * tmp80 - 1. / 2. * tmp81 - 1. / 2. * tmp82 -
                    1. / 2. * tmp84 - 1. / 2. * tmp86 - 1. / 2. * tmp88 -
                    1. / 2. * tmp90 + (1. / 2.) * tmp91 + (1. / 2.) * tmp92 +
                    (1. / 2.) * tmp93 + (1. / 2.) * tmp94;
                auto tmp97 = state_inv.x() * tmp21 + state_inv.z() * tmp24;
                auto tmp98 = state_inv.w() * tmp26;
                auto tmp99 = state_inv.y() * tmp19;
                auto tmp100 = state_inv.y() * tmp29;
                auto tmp101 = state_inv.w() * tmp35;
                auto tmp102 = state_inv.z() * tmp33;
                auto tmp103 = state_inv.x() * tmp31;
                auto tmp104 = state_inv.z() * tmp79;
                auto tmp105 = state_inv.x() * tmp8;
                auto tmp106 = state_inv.z() * tmp37;
                auto tmp107 = state_inv.x() * tmp46;
                auto tmp108 = state_inv.z() * tmp53;
                auto tmp109 = state_inv.y() * tmp83;
                auto tmp110 = q.w() * q.z();
                auto tmp111 = tmp110 * tmp2;
                auto tmp112 = q.x() * q.y();
                auto tmp113 = tmp1 * tmp112;
                auto tmp114 = state_inv.y() * tmp5;
                auto tmp115 = state_inv.z() * tmp85;
                auto tmp116 = tmp110 * tmp3;
                auto tmp117 = tmp0 * tmp112;
                auto tmp118 = tmp100 + tmp101 - tmp102 - tmp103 + tmp104 +
                              tmp105 + tmp106 + tmp107 + tmp108 + tmp109 +
                              tmp111 + tmp113 - tmp114 - tmp115 - tmp116 -
                              tmp117;
                auto tmp119 =
                    -1. / 2. * tmp100 - 1. / 2. * tmp101 + (1. / 2.) * tmp102 +
                    (1. / 2.) * tmp103 - 1. / 2. * tmp104 - 1. / 2. * tmp105 -
                    1. / 2. * tmp106 - 1. / 2. * tmp107 - 1. / 2. * tmp108 -
                    1. / 2. * tmp109 - 1. / 2. * tmp111 - 1. / 2. * tmp113 +
                    (1. / 2.) * tmp114 + (1. / 2.) * tmp115 +
                    (1. / 2.) * tmp116 + (1. / 2.) * tmp117 + tmp118 * tmp66;
                auto tmp120 = q.z() * state_inv.x() * tmp18 * tmp58 * tmp59;
                auto tmp121 = q.w() * state_inv.y() * tmp18 * tmp58 * tmp59;
                auto tmp122 = q.x() * state_inv.z() * tmp18 * tmp58 * tmp59;
                auto tmp123 = q.y() * state_inv.w() * tmp18 * tmp58 * tmp59;
                auto tmp124 = 2 * q.w() * state_inv.y() * tmp17 * tmp64;
                auto tmp125 = 2 * q.x() * state_inv.z() * tmp17 * tmp64;
                auto tmp126 = 2 * q.y() * state_inv.w() * tmp17 * tmp64;
                auto tmp127 = 2 * q.z() * state_inv.x() * tmp17 * tmp64;
                auto tmp128 = -tmp25;
                auto tmp129 = state_inv.y() * tmp21 + state_inv.z() * tmp26;
                auto tmp130 = state_inv.x() * tmp19;
                auto tmp131 = state_inv.w() * tmp24;
                auto tmp132 = q.x() * state_inv.y() * tmp18 * tmp58 * tmp59;
                auto tmp133 = q.w() * state_inv.z() * tmp18 * tmp58 * tmp59;
                auto tmp134 = q.y() * state_inv.x() * tmp18 * tmp58 * tmp59;
                auto tmp135 = q.z() * state_inv.w() * tmp18 * tmp58 * tmp59;
                auto tmp136 = 2 * q.w() * state_inv.z() * tmp17 * tmp64;
                auto tmp137 = 2 * q.x() * state_inv.y() * tmp17 * tmp64;
                auto tmp138 = 2 * q.y() * state_inv.x() * tmp17 * tmp64;
                auto tmp139 = 2 * q.z() * state_inv.w() * tmp17 * tmp64;
                Eigen::Matrix<double, 3, 3> ret;
                ret << tmp23 + tmp25 + tmp28 + tmp57 * tmp60 - tmp57 * tmp61 -
                           tmp57 * tmp62 - tmp57 * tmp63 + tmp65 * tmp67 +
                           tmp67 * tmp68 - tmp67 * tmp69 + tmp67 * tmp70,
                    tmp60 * tmp95 - tmp61 * tmp95 - tmp62 * tmp95 -
                        tmp63 * tmp95 + tmp65 * tmp96 + tmp68 * tmp96 -
                        tmp69 * tmp96 + tmp70 * tmp96 + tmp71 + tmp72 - tmp73,
                    tmp118 * tmp60 - tmp118 * tmp61 - tmp118 * tmp62 -
                        tmp118 * tmp63 + tmp119 * tmp65 + tmp119 * tmp68 -
                        tmp119 * tmp69 + tmp119 * tmp70 + tmp97 + tmp98 - tmp99,
                    tmp120 * tmp57 - tmp121 * tmp57 - tmp122 * tmp57 -
                        tmp123 * tmp57 + tmp124 * tmp67 + tmp125 * tmp67 +
                        tmp126 * tmp67 - tmp127 * tmp67 + tmp71 - tmp72 + tmp73,
                    tmp120 * tmp95 - tmp121 * tmp95 - tmp122 * tmp95 -
                        tmp123 * tmp95 + tmp124 * tmp96 + tmp125 * tmp96 +
                        tmp126 * tmp96 - tmp127 * tmp96 + tmp128 + tmp23 +
                        tmp27,
                    tmp118 * tmp120 - tmp118 * tmp121 - tmp118 * tmp122 -
                        tmp118 * tmp123 + tmp119 * tmp124 + tmp119 * tmp125 +
                        tmp119 * tmp126 - tmp119 * tmp127 + tmp129 + tmp130 -
                        tmp131,
                    tmp132 * tmp57 - tmp133 * tmp57 - tmp134 * tmp57 -
                        tmp135 * tmp57 + tmp136 * tmp67 - tmp137 * tmp67 +
                        tmp138 * tmp67 + tmp139 * tmp67 + tmp97 - tmp98 + tmp99,
                    tmp129 - tmp130 + tmp131 + tmp132 * tmp95 - tmp133 * tmp95 -
                        tmp134 * tmp95 - tmp135 * tmp95 + tmp136 * tmp96 -
                        tmp137 * tmp96 + tmp138 * tmp96 + tmp139 * tmp96,
                    tmp118 * tmp132 - tmp118 * tmp133 - tmp118 * tmp134 -
                        tmp118 * tmp135 + tmp119 * tmp136 - tmp119 * tmp137 +
                        tmp119 * tmp138 + tmp119 * tmp139 + tmp128 + tmp20 +
                        tmp22 + tmp28;
                return ret;
            }
        };
    } // namespace

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
