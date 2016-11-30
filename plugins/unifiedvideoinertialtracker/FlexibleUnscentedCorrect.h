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

#ifndef INCLUDED_FlexibleUnscentedCorrect_h_GUID_21E01E3B_5BD0_4F85_3B75_BBF6C657DBB4
#define INCLUDED_FlexibleUnscentedCorrect_h_GUID_21E01E3B_5BD0_4F85_3B75_BBF6C657DBB4

// Internal Includes
#include "SigmaPointGenerator.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {

    template <typename State, typename Measurement>
    class SigmaPointCorrectionApplication {
      public:
        static const types::DimensionType StateDim =
            types::Dimension<State>::value;
        static const types::DimensionType MeasurementDim =
            types::Dimension<Measurement>::value;

        using StateVec = types::DimVector<State>;
        using StateSquareMatrix = types::DimSquareMatrix<State>;
        using MeasurementSquareMatrix = types::DimSquareMatrix<Measurement>;

        /// state augmented with measurement noise mean
        static const types::DimensionType AugmentedStateDim =
            StateDim + MeasurementDim;
        using AugmentedStateVec = types::Vector<AugmentedStateDim>;
        using AugmentedStateCovMatrix = types::SquareMatrix<AugmentedStateDim>;
        using SigmaPointsGen =
            AugmentedSigmaPointGenerator<AugmentedStateDim, StateDim>;

        static const types::DimensionType NumSigmaPoints =
            SigmaPointsGen::NumSigmaPoints;

        using Reconstruction =
            ReconstructedDistributionFromSigmaPoints<MeasurementDim,
                                                     SigmaPointsGen>;
        using TransformedSigmaPointsMat =
            typename Reconstruction::TransformedSigmaPointsMat;

        using GainMatrix = types::Matrix<StateDim, MeasurementDim>;

        SigmaPointCorrectionApplication(State &s, Measurement &m,
                                        SigmaPointParameters params)
            : state(s), measurement(m),
              sigmaPoints(getAugmentedStateVec(s, m),
                          getAugmentedStateCov(s, m), params),
              transformedPoints(
                  transformSigmaPoints(state, measurement, sigmaPoints)),
              reconstruction(sigmaPoints, transformedPoints),
              innovationCovariance(computeInnovationCovariance(
                  state, measurement, reconstruction)),
              K(computeKalmanGain(innovationCovariance, reconstruction)),
              stateCorrection(K * reconstruction.getMean()) {}

        static AugmentedStateVec getAugmentedStateVec(State const &s,
                                                      Measurement const &m) {
            AugmentedStateVec ret;
            /// assuming measurement noise is zero mean
            ret << s.stateVector(), types::DimVector<Measurement>::Zero();
            return ret;
        }

        static AugmentedStateCovMatrix getAugmentedStateCov(State const &s,
                                                            Measurement &m) {
            AugmentedStateCovMatrix ret;
            ret << s.errorCovariance(),
                types::Matrix<StateDim, MeasurementDim>::Zero(),
                types::Matrix<MeasurementDim, StateDim>::Zero(),
                m.getCovariance(s);
            return ret;
        }

        /// Transforms sigma points by having the measurement compute the
        /// residual for a state whose state vector we update to each of the
        /// sigma points in turn.
        static TransformedSigmaPointsMat
        transformSigmaPoints(State const &s, Measurement &m,
                             SigmaPointsGen const &sigmaPoints) {
            TransformedSigmaPointsMat ret;
            State tempS = s;
            for (std::size_t i = 0; i < NumSigmaPoints; ++i) {
                tempS.setStateVector(sigmaPoints.getSigmaPoint(i));
                ret.col(i) = m.getResidual(tempS);
            }
            return ret;
        }

        static MeasurementSquareMatrix
        computeInnovationCovariance(State const &s, Measurement &m,
                                    Reconstruction const &recon) {
            return recon.getCov() + m.getCovariance(s);
        }

        static GainMatrix computeKalmanGain(MeasurementSquareMatrix const &Pvv,
                                            Reconstruction const &recon) {

            // Solve for K in K=Pxy Pvv^-1
            // where the cross-covariance matrix from the reconstruction is
            // transpose(Pxy) and Pvv is the reconstructed covariance plus the
            // measurement covariance
            // (Actually solves with transpose(Pvv) * transpose(K) =
            // transpose(Pxy) )
            GainMatrix ret = Pvv.transpose().ldlt().solve(recon.getCrossCov());
            return ret;
        }

        void finishCorrection() {
            state.setStateVector(state.stateVector() + stateCorrection);

            StateSquareMatrix newP = state.errorCovariance() -
                                     K * innovationCovariance * K.transpose();
            state.setErrorCovariance(newP);
            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state.postCorrect();
        }

        State &state;
        Measurement &measurement;
        SigmaPointsGen sigmaPoints;
        TransformedSigmaPointsMat transformedPoints;
        Reconstruction reconstruction;
        MeasurementSquareMatrix innovationCovariance;
        GainMatrix K;
        StateVec stateCorrection;
    };
    template <typename State, typename Measurement>
    inline SigmaPointCorrectionApplication<State, Measurement>
    beginUnscentedCorrection(State &s, Measurement &m,
                             SigmaPointParameters params) {
        return SigmaPointCorrectionApplication<State, Measurement>(s, m,
                                                                   params);
    }
} // namespace kalman
} // namespace osvr
#endif // INCLUDED_FlexibleUnscentedCorrect_h_GUID_21E01E3B_5BD0_4F85_3B75_BBF6C657DBB4
