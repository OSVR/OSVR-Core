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

#ifndef INCLUDED_StateHistory_h_GUID_07D9DAEE_6733_46AA_772C_43A51B2BAC10
#define INCLUDED_StateHistory_h_GUID_07D9DAEE_6733_46AA_772C_43A51B2BAC10

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <osvr/Kalman/FlexibleKalmanBase.h>

// Standard includes
#include <array>
#include <vector>
#include <type_traits>

namespace osvr {
namespace kalman {
    namespace pose_externalized_rotation {
        // forward declaration
        class State;
    } // namespace pose_externalized_rotation
    namespace orient_externalized_rotation {
        // forward declaration
        class State;
    }
} // namespace kalman
} // namespace osvr

namespace osvr {
namespace vbtracker {

    namespace detail {
        /// Does the specified state type also have an "external" quaternion we
        /// should save and restore?
        template <typename StateType>
        struct StateHasExternalQuaternion : std::false_type {};
        template <>
        struct StateHasExternalQuaternion<
            kalman::pose_externalized_rotation::State> : std::true_type {};
        template <>
        struct StateHasExternalQuaternion<
            kalman::orient_externalized_rotation::State> : std::true_type {};

        /// Base state history entry - handles standard states with everything
        /// in the state vector and error covariance.
        template <typename State> class StateHistoryEntryBase {
            using StateVec = kalman::types::DimVector<State>;
            using StateMatrix = kalman::types::DimSquareMatrix<State>;

            // Using std::array here so that we can easily stick this in a
            // vector
            // without worrying about alignment.
            using StateDim = kalman::types::Dimension<State>;
            using StateVectorBackup =
                std::array<kalman::types::Scalar, StateDim::value>;
            using StateCovarianceBackup =
                std::array<kalman::types::Scalar,
                           StateDim::value * StateDim::value>;

          public:
            /// Constructor - saves the state vector and error covariance
            explicit StateHistoryEntryBase(State const &state) {
                StateVec::Map(m_stateVector.data()) = state.stateVector();
                StateMatrix::Map(m_covariance.data()) = state.errorCovariance();
            }

            void restore(State &state) const {
                state.setStateVector(StateVec::Map(m_stateVector.data()));
                state.setErrorCovariance(StateMatrix::Map(m_covariance.data()));
            }

          private:
            util::time::TimeValue m_timestamp;
            StateVectorBackup m_stateVector;
            StateCovarianceBackup m_covariance;
        };
        struct NoExternalState;
        struct HasExternalQuaternion;
        /// Select the tag for tag dispatching.
        template <typename State>
        using StateHistoryEntryTagSelector = typename std::conditional<
            detail::StateHasExternalQuaternion<State>::value,
            HasExternalQuaternion, NoExternalState>::type;
    } // namespace detail

    /// State history entry template - default implementation is for those that
    /// don't have added extras, just forwards directly to the Base.
    ///
    /// Uses tag dispatching to choose an alternate implementation if required.
    template <typename State,
              typename Tag = detail::StateHistoryEntryTagSelector<State>>
    class StateHistoryEntry : public detail::StateHistoryEntryBase<State> {
        using Base = detail::StateHistoryEntryBase<State>;

      public:
        explicit StateHistoryEntry(State const &state) : Base(state) {}
    };

    template <typename State>
    class StateHistoryEntry<State, detail::HasExternalQuaternion> {
        using BaseEntry = detail::StateHistoryEntryBase<State>;

      public:
        explicit StateHistoryEntry(State const &state) : m_baseEntry(state) {
            /// also save the quat.
            Eigen::Vector4d::Map(m_quatBackup.data()) =
                state.getQuaternion().coeffs();
        }

        void restore(State &state) const {
            m_baseEntry.restore(state);
            /// also restore the quat.
            Eigen::Quaterniond quat;
            quat.coeffs() = Eigen::Vector4d::Map(m_quatBackup.data());
            state.setQuaternion(quat);
        }

      private:
        BaseEntry m_baseEntry;
        std::array<kalman::types::Scalar, 4> m_quatBackup;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_StateHistory_h_GUID_07D9DAEE_6733_46AA_772C_43A51B2BAC10
