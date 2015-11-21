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

#ifndef INCLUDED_AugmentedState_h_GUID_7A7BD6AE_F672_4096_679B_D5BB21D42445
#define INCLUDED_AugmentedState_h_GUID_7A7BD6AE_F672_4096_679B_D5BB21D42445

// Internal Includes
#include "FlexibleKalmanBase.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace kalman {
    template <typename StateA, typename StateB> class AugmentedState {
      public:
        using StateTypeA = StateA;
        using StateTypeB = StateB;

        static const types::DimensionType DIM_A =
            types::Dimension<StateA>::value;
        static const types::DimensionType DIM_B =
            types::Dimension<StateB>::value;
        static const types::DimensionType DIMENSION = DIM_A + DIM_B;

        using SquareMatrix = types::SquareMatrix<DIMENSION>;
        using StateVector = types::Vector<DIMENSION>;
        AugmentedState(StateA &a, StateB &b) : a_(a), b_(b) {}
        AugmentedState(AugmentedState const &other) = default;
        AugmentedState(AugmentedState &&other) : a_(other.a_), b_(other.b_) {}

        void setStateVector(StateVector const &state) {
            a().setStateVector(state.head<DIM_A>());
            b().setStateVector(state.tail<DIM_B>());
        }

        StateVector stateVector() const {
            StateVector ret;
            ret << a().stateVector(), b().stateVector();
            return ret;
        }

        SquareMatrix errorCovariance() const {
            SquareMatrix ret = SquareMatrix::Zero();
            ret.topLeftCorner<DIM_A, DIM_A>() = a().errorCovariance();
            ret.bottomRightCorner<DIM_B, DIM_B>() = b().errorCovariance();
        }

        void setErrorCovariance(SquareMatrix const &P) {
            a().setErrorCovariance(P.topLeftCorner<DIM_A, DIM_A>());
            b().setErrorCovariance(P.bottomRightCorner<DIM_B, DIM_B>());
        }

        void postCorrect() {
            a().postCorrect();
            b().postCorrect();
        }

        StateTypeA &a() { return a_; }
        StateTypeA const &a() const { return a_; }

        StateTypeB &b() { return b_; }
        StateTypeB const &b() const { return b_; }

      private:
        StateA &a_;
        StateB &b_;
    };

    template <typename StateA, typename StateB>
    inline AugmentedState<StateA, StateB> augmentedState(StateA &a, StateB &b) {
        return AugmentedState<StateA, StateB>(a, b);
    }

} // namespace kalman
} // namespace osvr

#endif // INCLUDED_AugmentedState_h_GUID_7A7BD6AE_F672_4096_679B_D5BB21D42445
