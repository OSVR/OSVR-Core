/** @file
    @brief Implementation

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

// Internal Includes
#include "TrackedBody.h"
#include "TrackedBodyIMU.h"
#include "TrackedBodyTarget.h"
#include "TrackingSystem.h"
#include "BodyTargetInterface.h"

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    struct TrackedBody::Impl {
        bool hasTime = false;
        osvr::util::time::TimeValue latest;
        osvr::util::time::TimeValue stateTime;
    };
    TrackedBody::TrackedBody(TrackingSystem &system, BodyId id)
        : m_system(system), m_id(id), m_impl(new Impl) {
        using StateVec = kalman::types::DimVector<BodyState>;
        /// Set error covariance matrix diagonal to large values for safety.
        m_state.setErrorCovariance(StateVec::Constant(10).asDiagonal());

        m_processModel.setDamping(getParams().linearVelocityDecayCoefficient,
                                  getParams().angularVelocityDecayCoefficient);
        m_processModel.setNoiseAutocorrelation(
            kalman::types::Vector<6>(getParams().processNoiseAutocorrelation));
    }

    TrackedBody::~TrackedBody() {}

    TrackedBodyIMU *TrackedBody::createIntegratedIMU() {
        /// @todo
        return nullptr;
    }

    TrackedBodyTarget *
    TrackedBody::createTarget(Eigen::Isometry3d const &targetToBody,
                              TargetSetupData const &setupData) {
        if (m_target) {
            // already have a target!
            /// @todo handle multiple targets!
            return nullptr;
        }
        /// The target will always be target 0...
        m_target.reset(
            new TrackedBodyTarget{*this, BodyTargetInterface{getState()},
                                  targetToBody, setupData, TargetId{0}});
        return m_target.get();
    }

    ConfigParams const &TrackedBody::getParams() const {
        return m_system.getParams();
    }

    BodyId TrackedBody::getId() const { return m_id; }

    void TrackedBody::adjustToMeasurementTime(
        osvr::util::time::TimeValue const &tv) {
        /// @todo implement the stack of IMU stuff.
        if (m_impl->hasTime) {
            /// Already had a time thing happen.
            auto dt = osvrTimeValueDurationSeconds(&tv, &m_impl->stateTime);
            if (tv != m_impl->stateTime) {
                /// Right now predicting backwards and forwards...
                kalman::predict(m_state, m_processModel, dt);
                m_state.postCorrect();
            }
            if (tv < m_impl->latest) {
                /// The new measurement is in the "past", oh dear.
                /// We should roll back, but until then...
                std::cout << "Had to go back in time!" << std::endl;
                // predict backwards
                m_impl->stateTime = tv;

            } else {
                /// Moving to the future.
                m_impl->stateTime = m_impl->latest = tv;
            }
        } else {
            /// This is our first timestamp.
            m_impl->hasTime = true;
            m_impl->stateTime = m_impl->latest = tv;
        }
    }

    void TrackedBody::replayRewoundMeasurements() {
        /// @todo No stack of history yet, so nothing to replay yet. Right now
        /// just predicting both directions :-/
        BOOST_ASSERT_MSG(m_impl->hasTime, "Only makes sense to call Replay "
                                          "once you already called Adjust and "
                                          "thus already have a time entered!");
        if (m_impl->stateTime < m_impl->latest) {
            /// "return" to where we were by means of prediction. :-/
            auto dt = osvrTimeValueDurationSeconds(&m_impl->latest,
                                                   &m_impl->stateTime);
            kalman::predict(m_state, m_processModel, dt);
            m_state.postCorrect();
        }
    }

    osvr::util::time::TimeValue TrackedBody::getStateTime() const {
        return m_impl->stateTime;
    }

} // namespace vbtracker
} // namespace osvr
