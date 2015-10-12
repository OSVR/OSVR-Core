/** @file
    @brief Header defining some filters for Eigen datatypes.

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

#ifndef INCLUDED_EigenFilters_h_GUID_ECD959BF_587B_4ABB_30B3_E881B4520AC8
#define INCLUDED_EigenFilters_h_GUID_ECD959BF_587B_4ABB_30B3_E881B4520AC8

// Internal Includes
// - none

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
#include <cmath>

/// @brief Filters for use with EIgen datatypes.
namespace filters {
/// @brief Low pass filter class and supporting free functions.
namespace low_pass {
    template <typename Derived>
    inline Derived computeStep(Eigen::MatrixBase<Derived> const &hatx,
                               Eigen::MatrixBase<Derived> const &x,
                               typename Derived::Scalar alpha) {
        return alpha * x + (1 - alpha) * hatx;
    }

    template <typename Derived>
    inline Derived computeStep(Eigen::QuaternionBase<Derived> const &hatx,
                               Eigen::QuaternionBase<Derived> const &x,
                               typename Derived::Scalar alpha) {
        return hatx.slerp(alpha, x).normalized();
    }

    /// Low pass filter (designed for use within the One Euro filter) that works
    /// with Eigen types.
    ///
    /// Requires that a `computeStep()` overload exist that can take the
    /// data type.
    template <typename T> class LowPassFilter {
      public:
        using value_type = T;
        using scalar = typename T::Scalar;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

        /// Feeds in new data with the specified alpha.
        /// @returns the new hatx
        value_type const &filter(Eigen::Ref<const value_type> const &x,
                                 scalar alpha) {
            if (m_firstTime) {
                m_firstTime = false;
                m_hatx = x;
                return m_hatx;
            }

            m_hatx = computeStep(hatx(), x, alpha);
            return m_hatx;
        }

        value_type const &hatx() const { return m_hatx; }

      private:
        value_type m_hatx;
        bool m_firstTime = true;
    };

} // namespace low_pass
  /// @brief "One-Euro" filter class and supporting structs and free functions.
namespace one_euro {

    namespace detail {
        /// Computing the alpha value for a step in the one euro filter for any
        /// scalar type.
        template <typename T> T computeAlpha(T dt, T cutoff) {
            auto tau = T(1) / (T(2) * M_PI * cutoff);
            return T(1) / (T(1) + tau / dt);
        }

    } // namespace detail

    /// Parameters needed for the one-euro filter
    struct Params {
        double minCutoff;
        double beta;
        double derivativeCutoff;
    };

    /// @name One-Euro Filter required functions for Eigen::Vector3d
    /// @{
    void setDerivativeIdentity(Eigen::Vector3d &dx) {
        dx = Eigen::Vector3d::Zero();
    }
    Eigen::Vector3d computeDerivative(Eigen::Vector3d const &prev,
                                      Eigen::Vector3d const &curr, double dt) {
        return (curr - prev) / dt;
    }
    double computeDerivativeMagnitude(Eigen::Vector3d const &dx) {
        return dx.norm();
    }
    /// @}

    /// @name One-Euro Filter required functions for Eigen::Quaterniond
    /// @{
    void setDerivativeIdentity(Eigen::Quaterniond &dx) {
        dx = Eigen::Quaterniond::Identity();
    }
    Eigen::Quaterniond computeDerivative(Eigen::Quaterniond const &prev,
                                         Eigen::Quaterniond const &curr,
                                         double dt) {
        // slerp, based on dt, between the identity and our difference
        // rotation.
        return Eigen::Quaterniond::Identity()
            .slerp(dt, curr * prev.inverse())
            .normalized();
    }
    double computeDerivativeMagnitude(Eigen::Quaterniond const &quat) {
        return 2.0 * std::acos(quat.w());
    }
    /// @}

    /// A simple filter designed for human input sources: high accuracy at
    /// low velocity, low latency at high velocity.
    ///
    /// See http://hal.inria.fr/hal-00670496/
    ///
    /// @tparam T the Eigen datatype you're using
    ///
    /// Design of the interface for this class inspired by the
    /// implementation in VRPN.
    template <typename T> class OneEuroFilter {
      public:
        using value_type = T;
        using scalar = typename T::Scalar;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
        explicit OneEuroFilter(Params const &p) : m_params(p) {}

        value_type const &filter(scalar dt,
                                 value_type const &x) {
            auto dx = value_type{};
            if (m_firstTime) {
                m_firstTime = false;
                detail::setDerivativeIdentity(dx);
            } else {
                dx = detail::computeDerivative(m_xFilter.hatx(), x, dt);
            }
            // Low-pass-filter the derivative.
            m_dxFilter.filter(dx, detail::computeAlpha<scalar>(
                                      dt, m_params.derivativeCutoff));
            // Get the magnitude of the (filtered) derivative
            auto dxMag = detail::computeDerivativeMagnitude(m_dxFilter.hatx());
            // Compute the cutoff to use for the x filter
            auto cutoff = m_params.minCutoff + m_params.beta * dxMag;

            // Filter the x and return the results.
            return m_xFilter.filter(x,
                                    detail::computeAlpha<scalar>(dt, cutoff));
        }

      private:
        bool m_firstTime = true;
        const Params m_params;
        low_pass::LowPassFilter<T> m_xFilter;
        low_pass::LowPassFilter<T> m_dxFilter;
    };
} // namespace one_euro
using one_euro::OneEuroFilter;
} // namespace filters

#endif // INCLUDED_EigenFilters_h_GUID_ECD959BF_587B_4ABB_30B3_E881B4520AC8
