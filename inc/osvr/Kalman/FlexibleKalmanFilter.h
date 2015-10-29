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

#ifndef INCLUDED_FlexibleKalmanFilter_h_GUID_3A3B8A14_6DFC_4A81_97CA_189834AC4B61
#define INCLUDED_FlexibleKalmanFilter_h_GUID_3A3B8A14_6DFC_4A81_97CA_189834AC4B61

// Internal Includes
#include "FlexibleKalmanBase.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    /// The main class implementing the common components of the Kalman family
    /// of filters. Holds an instance of the state as well as an instance of the
    /// process model.
    template <typename ProcessModelType,
              typename StateType = typename ProcessModelType::State>
    class FlexibleKalmanFilter {
      public:
        using State = StateType;
        using ProcessModel = ProcessModelType;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        /// Default initialization - depends on default-initializable process
        /// model and state.
        FlexibleKalmanFilter() : m_processModel(), m_state() {}

        /// Copy initialization from just state - depends on
        /// default-initializable process model.
        explicit FlexibleKalmanFilter(State const &state)
            : m_processModel(), m_state(state) {}

        /// Move initialization from just state - depends on
        /// default-initializable process model.
        explicit FlexibleKalmanFilter(State &&state)
            : m_processModel(), m_state(state) {}

        /// copy initialization
        FlexibleKalmanFilter(ProcessModel const &processModel,
                             State const &state)
            : m_processModel(processModel), m_state(state) {}

        /// move initialization.
        FlexibleKalmanFilter(ProcessModel &&processModel, State &&state)
            : m_processModel(processModel), m_state(state) {}

        void predict(double dt) { m_processModel.predictState(state(), dt); }

        template <typename MeasurementType>
        void correct(MeasurementType const &meas) {
            /// Dimension of measurement
            static const auto m = types::Dimension<MeasurementType>::value;
            /// Dimension of state
            static const auto n = types::Dimension<State>::value;
            auto H = meas.getJacobian(state());
            OSVR_KALMAN_DEBUG_OUTPUT("Measurement jacobian", H);
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(H), m, n);

            auto R = meas.getCovariance(state());
            OSVR_KALMAN_DEBUG_OUTPUT("Measurement covariance", R);
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(R), m, m);

            auto P = state().errorCovariance();
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(P), n, n);

            // the stuff to invert for the kalman gain
            // denom * K = numer
            types::SquareMatrix<m> denom = H * P * H.transpose() + R;

            // The kalman gain stuff to not invert
            types::Matrix<n, m> numer = P * H.transpose();

            // Solve for the Kalman gain
            /// @todo Figure out if this is the best decomp to use
            types::Matrix<n, m> K = denom.colPivHouseholderQr().solve(numer);
            // types::Matrix<n, m> K = denom.ldlt().solve(numer);
            // types::Matrix<n, m> K = numer * denom.inverse(); // this one
            // creates lots of nans.
            OSVR_KALMAN_DEBUG_OUTPUT("Kalman gain K", K);

            // Residual/innovation
            auto deltaz = meas.getResidual(state());
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(decltype(deltaz), m);

            OSVR_KALMAN_DEBUG_OUTPUT("deltaz", deltaz.transpose());

            OSVR_KALMAN_DEBUG_OUTPUT("state correction",
                                     (K * deltaz).transpose());

            types::Vector<n> correctedState =
                state().stateVector() + K * deltaz;
            // Correct the state estimate
            state().setStateVector(correctedState);
            // Correct the error covariance
            OSVR_KALMAN_DEBUG_OUTPUT("state error covariance correction factor",
                                     types::DimSquareMatrix<State>::Identity() -
                                         (K * H));
            state().setErrorCovariance(
                (types::DimSquareMatrix<State>::Identity() - K * H) * P);

            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state().postCorrect();
        }

        ProcessModel &processModel() { return m_processModel; }
        ProcessModel const &processModel() const { return m_processModel; }

        State &state() { return m_state; }
        State const &state() const { return m_state; }

      private:
        ProcessModel m_processModel;
        State m_state;
    };

} // namespace kalman
} // namespace osvr

#endif // INCLUDED_FlexibleKalmanFilter_h_GUID_3A3B8A14_6DFC_4A81_97CA_189834AC4B61
