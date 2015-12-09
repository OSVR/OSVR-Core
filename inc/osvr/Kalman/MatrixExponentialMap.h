/** @file
    @brief Header

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

#ifndef INCLUDED_ExponentialMap_h_GUID_FB61635E_CF8A_4FAE_5343_2258F4BC1E60
#define INCLUDED_ExponentialMap_h_GUID_FB61635E_CF8A_4FAE_5343_2258F4BC1E60

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace kalman {

    /// Produces the "hat matrix" that produces the same result as
    /// performing a cross-product with v. This is the same as the "capital
    /// omega" skew-symmetrix matrix used by a matrix-exponential-map
    /// rotation vector.
    /// @param v a 3D vector
    /// @return a matrix M such that for some 3D vector u, Mu = v x u.
    template <typename Derived>
    inline Eigen::Matrix3d
    makeSkewSymmetrixCrossProductMatrix(Eigen::MatrixBase<Derived> const &v) {
        Eigen::Matrix3d ret;
        // clang-format off
                ret << 0, -v.z(), v.y(),
                       v.z(), 0, -v.x(),
                       -v.y(), v.x(), 0;
        // clang-format on
        return ret;
    }

    /// Utilities for interacting with a "matrix exponential map vector"
    /// rotation parameterization/formalism, where rotation is represented as a
    /// 3D vector that is turned into a rotation matrix by applying Rodrigues'
    /// formula that resembles a matrix exponential.
    ///
    /// Based on discussion in section 2.2.3 of:
    ///
    /// Lepetit, V., & Fua, P. (2005). Monocular Model-Based 3D Tracking of
    /// Rigid Objects. Foundations and Trends® in Computer Graphics and Vision,
    /// 1(1), 1–89. http://doi.org/10.1561/0600000001
    ///
    /// Not to be confused with the quaternion-related exponential map espoused
    /// in:
    ///
    /// Grassia, F. S. (1998). Practical Parameterization of Rotations Using the
    /// Exponential Map. Journal of Graphics Tools, 3(3), 29–48.
    /// http://doi.org/10.1080/10867651.1998.10487493
    namespace matrix_exponential_map {
        /// Adjust a matrix exponential map rotation vector, if required, to
        /// avoid  singularities.
        /// @param omega a 3D "matrix exponential map" rotation vector, which
        /// will be modified if required.
        template <typename T> inline void avoidSingularities(T &&omega) {
            // if magnitude gets too close to 2pi, in this case, pi...
            if (omega.squaredNorm() > M_PI * M_PI) {
                // replace omega with an equivalent one.
                omega = ((1 - (2 * M_PI) / omega.norm()) * omega).eval();
            }
        }

        /// Gets the rotation angle of a rotation vector.
        /// @param omega a 3D "exponential map" rotation vector
        template <typename Derived>
        inline double getAngle(Eigen::MatrixBase<Derived> const &omega) {
            return omega.norm();
        }

        /// Gets the unit quaternion corresponding to the exponential rotation
        /// vector.
        /// @param omega a 3D "exponential map" rotation vector
        template <typename Derived>
        inline Eigen::Quaterniond
        getQuat(Eigen::MatrixBase<Derived> const &omega) {
            auto theta = getAngle(omega);
            auto xyz = omega * std::sin(theta / 2.);
            return Eigen::Quaterniond(std::cos(theta / 2.), xyz.x(), xyz.y(),
                                      xyz.z());
        }

        /// Contained cached computed values
        class ExponentialMapData {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            /// Construct from a matrixy-thing: should be a 3d vector containing
            /// a matrix-exponential-map rotation formalism.
            template <typename Derived>
            explicit ExponentialMapData(Eigen::MatrixBase<Derived> const &omega)
                : m_omega(omega) {}

            ExponentialMapData() : m_omega(Eigen::Vector3d::Zero()) {}

            /// assignment operator - its presence is an optimization only.
            ExponentialMapData &operator=(ExponentialMapData const &other) {
                if (&other != this) {
                    m_omega = other.m_omega;
                    m_gotTheta = other.m_gotTheta;
                    if (m_gotTheta) {
                        m_theta = other.m_theta;
                    }
                    m_gotBigOmega = other.m_gotBigOmega;
                    if (m_gotBigOmega) {
                        m_bigOmega = other.m_bigOmega;
                    }
                    m_gotR = other.m_gotR;
                    if (m_gotR) {
                        m_R = other.m_R;
                    }
                    m_gotQuat = other.m_gotQuat;
                    if (m_gotQuat) {
                        m_quat = other.m_quat;
                    }
                }
                return *this;
            }

            /// move-assignment operator - its presence is an optimization only.
            ExponentialMapData &operator=(ExponentialMapData &&other) {
                if (&other != this) {
                    m_omega = std::move(other.m_omega);
                    m_gotTheta = std::move(other.m_gotTheta);
                    if (m_gotTheta) {
                        m_theta = std::move(other.m_theta);
                    }
                    m_gotBigOmega = std::move(other.m_gotBigOmega);
                    if (m_gotBigOmega) {
                        m_bigOmega = std::move(other.m_bigOmega);
                    }
                    m_gotR = std::move(other.m_gotR);
                    if (m_gotR) {
                        m_R = std::move(other.m_R);
                    }
                    m_gotQuat = std::move(other.m_gotQuat);
                    if (m_gotQuat) {
                        m_quat = std::move(other.m_quat);
                    }
                }
                return *this;
            }

            template <typename Derived>
            void reset(Eigen::MatrixBase<Derived> const &omega) {
                /// Using assignment operator to be sure I didn't miss a flag.
                *this = ExponentialMapData(omega);
            }

            /// Gets the "capital omega" skew-symmetrix matrix.
            ///
            /// (computation is cached)
            Eigen::Matrix3d const &getBigOmega() {
                if (!m_gotBigOmega) {
                    m_gotBigOmega = true;
                    m_bigOmega = makeSkewSymmetrixCrossProductMatrix(m_omega);
                }
                return m_bigOmega;
            }

            /// Gets the rotation angle of a rotation vector.
            ///
            /// (computation is cached)
            double getTheta() {
                if (!m_gotTheta) {
                    m_gotTheta = true;
                    m_theta = getAngle(m_omega);
                }
                return m_theta;
            }

            /// Converts a rotation vector to a rotation matrix:
            /// Uses Rodrigues' formula, and the first two terms of the Taylor
            /// expansions of the trig functions (so as to be nonsingular as the
            /// angle goes to zero).
            ///
            /// (computation is cached)
            Eigen::Matrix3d const &getRotationMatrix() {
                if (!m_gotR) {
                    m_gotR = true;
                    auto theta = getTheta();
                    auto &Omega = getBigOmega();
                    /// two-term taylor approx of sin(theta)/theta
                    double k1 = 1. - theta * theta / 6.;

                    /// two-term taylor approx of (1-cos(theta))/theta
                    double k2 = theta / 2. - theta * theta * theta / 24.;

                    m_R = Eigen::Matrix3d::Identity() + k1 * Omega +
                          k2 * Omega * Omega;
                }
                return m_R;
            }

            Eigen::Quaterniond const &getQuaternion() {
                if (!m_gotQuat) {
                    m_gotQuat = true;
                    auto theta = getTheta();
                    auto xyz = m_omega * std::sin(theta / 2.);
                    m_quat = Eigen::Quaterniond(std::cos(theta / 2.), xyz.x(),
                                                xyz.y(), xyz.z());
                }
                return m_quat;
            }

          private:
            Eigen::Vector3d m_omega;
            bool m_gotTheta = false;
            double m_theta;
            bool m_gotBigOmega = false;
            Eigen::Matrix3d m_bigOmega;
            bool m_gotR = false;
            Eigen::Matrix3d m_R;
            bool m_gotQuat = false;
            Eigen::Quaterniond m_quat;
        };

    } // namespace exponential_map
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_ExponentialMap_h_GUID_FB61635E_CF8A_4FAE_5343_2258F4BC1E60
