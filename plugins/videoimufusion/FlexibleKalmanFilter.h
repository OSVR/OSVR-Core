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
    template <typename StateType, typename ProcessModelType>
    class FlexibleKalmanFilter {
      public:
        using State = StateType;
        using ProcessModel = ProcessModelType;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        static const std::size_t STATE_DIMENSION = StateType::DIMENSION;
        /// copy initialization
        FlexibleKalmanFilter(State const &state,
                             ProcessModel const &processModel)
            : m_state(state), m_processModel(processModel) {}
        /// move initialization.
        FlexibleKalmanFilter(State &&state, ProcessModel &&processModel)
            : m_state(state), m_processModel(processModel) {}

        void predict(double dt) { m_processModel.predictState(state(), dt); }

        template <typename MeasurementType>
        void correct(MeasurementType const &meas) {
            /// Dimension of measurement
            static const auto m = types::Dimension<MeasurementType>::value;
            /// Dimension of state
            static const auto n = types::Dimension<State>::value;
            auto H = meas.getJacobian(state());
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(H), m, n);

            auto R = meas.getCovariance(state());
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(R), m, m);

            auto P = state().errorCovariance();
            EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(decltype(P), n, n);

            // the stuff to invert for the kalman gain
            // denom * K = numer
            types::SquareMatrix<m> denom = H * P * H.transpose() + R;

            // The kalman gain stuff to not invert
            types::Matrix<n, m> numer = P * H.transpose();
            types::Matrix<n, m> K = denom.colPivHouseholderQr().solve(numer);

            // Residual/innovation
            auto deltaz = meas.getResidual(state());
            EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(decltype(deltaz), m);

            types::Vector<n> correctedState =
                state().stateVector() + K * deltaz;
            // Correct the state estimate
            state().setStateVector(correctedState);
            // Correct the error covariance
            state().setErrorCovariance(
                (types::DimSquareMatrix<State>::Identity() - K * H) * P);

            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state().postCorrect();
        }

        State &state() { return m_state; }
        State const &state() const { return m_state; }
        ProcessModel &processModel() { return m_processModel; }
        ProcessModel const &processModel() const { return m_processModel; }

      private:
        State m_state;
        ProcessModel m_processModel;
    };

} // namespace kalman
} // namespace osvr

#endif // INCLUDED_FlexibleKalmanFilter_h_GUID_3A3B8A14_6DFC_4A81_97CA_189834AC4B61
