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
    void predict(StateType &state, ProcessModelType &processModel, double dt) {
        processModel.predictState(state, dt);
        OSVR_KALMAN_DEBUG_OUTPUT("Predicted state",
                                 state.stateVector().transpose());

        OSVR_KALMAN_DEBUG_OUTPUT("Predicted error covariance",
                                 state.errorCovariance());
    }

    template <typename StateType, typename ProcessModelType,
              typename MeasurementType>
    inline void correct(StateType &state, ProcessModelType &processModel,
                        MeasurementType &meas) {
        /// Dimension of measurement
        static const auto m = types::Dimension<MeasurementType>::value;
        /// Dimension of state
        static const auto n = types::Dimension<StateType>::value;

        types::Matrix<m, n> H = meas.getJacobian(state);
        // OSVR_KALMAN_DEBUG_OUTPUT("Measurement jacobian", H);

        types::SquareMatrix<m> R = meas.getCovariance(state);
        // OSVR_KALMAN_DEBUG_OUTPUT("Measurement covariance", R);

        types::SquareMatrix<n> P = state.errorCovariance();

        // The kalman gain stuff to not invert (called P12 in TAG)
        types::Matrix<n, m> PHt = P * H.transpose();
        // OSVR_KALMAN_DEBUG_OUTPUT("PHt/numerator", P * H.transpose());

        // the stuff to invert for the kalman gain
        // also sometimes called S or the "Innovation Covariance"
        types::SquareMatrix<m> S = H * PHt + R;
        // OSVR_KALMAN_DEBUG_OUTPUT("Transformed covariance", H * PHt);
        // OSVR_KALMAN_DEBUG_OUTPUT("S: Innovation covariance", H * PHt + R);

        // Not going to directly compute Kalman gain K = PHt (S^-1)
        // Instead, decomposed S to solve things of the form (S^-1)x
        // repeatedly later, by using the substitution
        // Kx = PHt denom.solve(x)

        // Eigen::ColPivHouseholderQR<types::SquareMatrix<m>> denom(S);
        /// @todo Figure out if this is the best decomp to use
        // TooN/TAG use this one, and others online seem to suggest it.
        Eigen::LDLT<types::SquareMatrix<m>> denom(S);
#if 0
        // Solve for the Kalman gain
        types::Matrix<n, m> K = denom.solve(PHt);
        // types::Matrix<n, m> K = denom.ldlt().solve(numer);
        // types::Matrix<n, m> K = numer * denom.inverse(); // this one
        // creates lots of nans.
        OSVR_KALMAN_DEBUG_OUTPUT("Kalman gain K", K);
#endif

        // Residual/innovation
        auto deltaz = meas.getResidual(state);
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(decltype(deltaz), m);
        OSVR_KALMAN_DEBUG_OUTPUT("deltaz", deltaz.transpose());

        types::Vector<n> stateCorrection = PHt * denom.solve(deltaz);
        OSVR_KALMAN_DEBUG_OUTPUT("state correction",
                                 (PHt * denom.solve(deltaz)).transpose());

        // Correct the state estimate
        state.setStateVector(state.stateVector() + stateCorrection);

#if 1
        // Correct the error covariance
        // differs from the (I-KH)P form by not factoring out the P (since
        // we already have PHt computed).
        OSVR_KALMAN_DEBUG_OUTPUT("error covariance difference",
                                 (PHt * denom.solve(PHt.transpose())));
        types::SquareMatrix<n> newP = P - (PHt * denom.solve(PHt.transpose()));
#else
        // Test fails with this one:
        // VariedProcessModelStability/1.AbsolutePoseMeasurementXlate111,
        // where TypeParam =
        // osvr::kalman::PoseDampedConstantVelocityProcessModel
        OSVR_KALMAN_DEBUG_OUTPUT(
            "error covariance scale",
            (types::SquareMatrix<n>::Identity() - PHt * denom.solve(H)));
        types::SquareMatrix<n> newP =
            (types::SquareMatrix<n>::Identity() - PHt * denom.solve(H)) * P;

#endif

#if 0
        // doesn't seem to be necessary to re-symmetrize the covariance matrix.
        state.setErrorCovariance((newP + newP.transpose()) / 2.);
#else
        state.setErrorCovariance(newP);
#endif

        // Let the state do any cleanup it has to (like fixing externalized
        // quaternions)
        state.postCorrect();
    }

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

        void predict(double dt) {
            kalman::predict(state(), processModel(), dt);
        }

        template <typename MeasurementType>
        void correct(MeasurementType &meas) {
            kalman::correct(state(), processModel(), meas);
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
