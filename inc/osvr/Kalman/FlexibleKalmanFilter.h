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
#include "FlexibleKalmanCorrect.h"

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

    /// @param cancelIfNotFinite If the state correction or new error covariance
    /// is detected to contain non-finite values, should we cancel the
    /// correction and not apply it?
    ///
    /// @return true if correction completed
    template <typename StateType, typename ProcessModelType,
              typename MeasurementType>
    inline bool correct(StateType &state, ProcessModelType &processModel,
                        MeasurementType &meas, bool cancelIfNotFinite = true) {

        auto inProgress = beginCorrection(state, processModel, meas);
        if (cancelIfNotFinite && !inProgress.stateCorrectionFinite) {
            return false;
        }

        return inProgress.finishCorrection(cancelIfNotFinite);
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
