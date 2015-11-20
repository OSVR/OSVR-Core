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

#ifndef INCLUDED_OrientationMeasurement_h_GUID_9820D71B_C9A1_468F_B591_6E7C837EEA0E
#define INCLUDED_OrientationMeasurement_h_GUID_9820D71B_C9A1_468F_B591_6E7C837EEA0E


// Internal Includes
#include "FlexibleKalmanBase.h"
#include "OrientationState.h"
#include "ExternalQuaternion.h"
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    class OrientationBase {
      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType DIMENSION = 4;
        using MeasurementVector = types::Vector<DIMENSION>;
        using MeasurementSquareMatrix = types::SquareMatrix<DIMENSION>;
        OrientationBase(Eigen::Quaterniond const &quat,
                                types::Vector<3> const &eulerVariance)
            : m_measurement(quat),
              m_covariance(MeasurementSquareMatrix::Identity()),
              m_eulerVariance(eulerVariance) {
            m_covariance.topLeftCorner<3, 3>().diagonal() =
                m_eulerVariance / 4.;

#if 1
            m_covariance(3, 3) = m_eulerVariance.sum();
#endif
        }

        template <typename State>
        MeasurementSquareMatrix const &getCovariance(State const &s) {
#if 0
            const types::Vector<3> q = getResidual(s).template head<3>();
            const types::Vector<3> q2 = (q.array() * q.array()).matrix();
            double denom = -16 * std::sqrt(1 - (q.dot(q) / 16.));
            const types::Vector<3> edge =
                (q.array() * m_eulerVariance.array() / denom).matrix();
            m_covariance.topRightCorner<3, 1>() = edge;
            m_covariance.bottomLeftCorner<1, 3>() = edge.transpose();
            m_covariance(3, 3) =
                (q2.dot(m_eulerVariance)) / (64. - 4. * (q2.sum()));
#endif
            return m_covariance;
        }

        /// Gets the measurement residual, also known as innovation: predicts
        /// the measurement from the predicted state, and returns the
        /// difference.
        ///
        /// State type doesn't matter as long as we can getQuaternion()
        template <typename State>
        MeasurementVector getResidual(State const &s) const {
            const Eigen::Quaterniond prediction = s.getQuaternion();
            const Eigen::Quaterniond residual =
                m_measurement * prediction.conjugate();
            return residual.coeffs();
        }
        /// Convenience method to be able to store and re-use measurements.
        void setMeasurement(Eigen::Quaterniond const &quat) {
            m_measurement = quat;
        }

      private:
        Eigen::Quaterniond m_measurement;
        MeasurementSquareMatrix m_covariance;
        types::Vector<3> m_eulerVariance;
    };

    /// This is the subclass of OrientationBase: only explicit
    /// specializations, and on state types.
    template <typename StateType> class OrientationMeasurement;

    /// OrientationMeasurement with a pose_externalized_rotation::State
    template <>
    class OrientationMeasurement<orient_externalized_rotation::State>
        : public OrientationBase {
      public:
        using State = orient_externalized_rotation::State;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const types::DimensionType STATE_DIMENSION =
            types::Dimension<State>::value;
        using Base = OrientationBase;

        OrientationMeasurement(Eigen::Quaterniond const &quat,
                                       types::Vector<3> const &eulerVariance)
            : Base(quat, eulerVariance) {}

        types::Matrix<DIMENSION, STATE_DIMENSION>
        getJacobian(State const &s) const {
            using namespace orient_externalized_rotation;
            using Jacobian = types::Matrix<DIMENSION, STATE_DIMENSION>;
            Jacobian ret = Jacobian::Zero();
            ret.block<DIMENSION, 3>(0, 3) = external_quat::jacobian(
                incrementalOrientation(s.stateVector()));
            return ret;
        }
    };
} // namespace kalman
} // namespace osvr

#endif // INCLUDED_OrientationMeasurement_h_GUID_9820D71B_C9A1_468F_B591_6E7C837EEA0E

