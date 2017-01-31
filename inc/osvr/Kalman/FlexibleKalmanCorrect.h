/** @file
    @brief Header

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

#ifndef INCLUDED_FlexibleKalmanCorrect_h_GUID_354B7B5B_CF4F_49AF_7F71_A4279BD8DA8C
#define INCLUDED_FlexibleKalmanCorrect_h_GUID_354B7B5B_CF4F_49AF_7F71_A4279BD8DA8C

// Internal Includes
#include "FlexibleKalmanBase.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    template <typename StateType, typename MeasurementType>
    struct CorrectionInProgress {
        /// Dimension of measurement
        static const types::DimensionType m =
            types::Dimension<MeasurementType>::value;
        /// Dimension of state
        static const types::DimensionType n =
            types::Dimension<StateType>::value;

        CorrectionInProgress(StateType &state, MeasurementType &meas,
                             types::SquareMatrix<n> const &P_,
                             types::Matrix<n, m> const &PHt_,
                             types::SquareMatrix<m> const &S)
            : P(P_), PHt(PHt_), denom(S), deltaz(meas.getResidual(state)),
              stateCorrection(PHt * denom.solve(deltaz)), state_(state),
              stateCorrectionFinite(stateCorrection.array().allFinite()) {}

        /// State error covariance
        types::SquareMatrix<n> P;

        /// The kalman gain stuff to not invert (called P12 in TAG)
        types::Matrix<n, m> PHt;

        /// Decomposition of S
        ///
        /// Not going to directly compute Kalman gain K = PHt (S^-1)
        /// Instead, decomposed S to solve things of the form (S^-1)x
        /// repeatedly later, by using the substitution
        /// Kx = PHt denom.solve(x)
        /// @todo Figure out if this is the best decomp to use
        // TooN/TAG use this one, and others online seem to suggest it.
        Eigen::LDLT<types::SquareMatrix<m>> denom;

        /// Measurement residual/delta z/innovation
        types::Vector<m> deltaz;

        /// Corresponding state change to apply.
        types::Vector<n> stateCorrection;

        /// Is the state correction free of NaNs and +- infs?
        bool stateCorrectionFinite;

        /// That's as far as we go here before you choose to continue.

        /// Finish computing the rest and correct the state.
        /// @param cancelIfNotFinite If the new error covariance is detected to
        /// contain non-finite values, should we cancel the correction and not
        /// apply it?
        /// @return true if correction completed
        bool finishCorrection(bool cancelIfNotFinite = true) {
            // Compute the new error covariance
            // differs from the (I-KH)P form by not factoring out the P (since
            // we already have PHt computed).
            types::SquareMatrix<n> newP =
                P - (PHt * denom.solve(PHt.transpose()));

#if 0
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

            if (!newP.array().allFinite()) {
                return false;
            }

            // Correct the state estimate
            state_.setStateVector(state_.stateVector() + stateCorrection);

            // Correct the error covariance
            state_.setErrorCovariance(newP);

#if 0
            // Doesn't seem necessary to re-symmetrize the covariance matrix.
            state_.setErrorCovariance((newP + newP.transpose()) / 2.);
#endif

            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state_.postCorrect();
            return true;
        }

      private:
        StateType &state_;
    };

    template <typename StateType, typename ProcessModelType,
              typename MeasurementType>
    inline CorrectionInProgress<StateType, MeasurementType>
    beginCorrection(StateType &state, ProcessModelType &processModel,
                    MeasurementType &meas) {

        /// Dimension of measurement
        static const auto m = types::Dimension<MeasurementType>::value;
        /// Dimension of state
        static const auto n = types::Dimension<StateType>::value;

        /// Measurement Jacobian
        types::Matrix<m, n> H = meas.getJacobian(state);

        /// Measurement covariance
        types::SquareMatrix<m> R = meas.getCovariance(state);

        /// State error covariance
        types::SquareMatrix<n> P = state.errorCovariance();

        /// The kalman gain stuff to not invert (called P12 in TAG)
        types::Matrix<n, m> PHt = P * H.transpose();

        /// the stuff to invert for the kalman gain
        /// also sometimes called S or the "Innovation Covariance"
        types::SquareMatrix<m> S = H * PHt + R;

        /// More computation is done in initializers/constructor
        return CorrectionInProgress<StateType, MeasurementType>(state, meas, P,
                                                                PHt, S);
    }

} // namespace kalman
} // namespace osvr

#endif // INCLUDED_FlexibleKalmanCorrect_h_GUID_354B7B5B_CF4F_49AF_7F71_A4279BD8DA8C
