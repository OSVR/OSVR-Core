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
#include "StateHistory.h"
#include "HistoryContainer.h"

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>
#include <boost/optional.hpp>

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    using BodyStateHistoryEntry = StateHistoryEntry<BodyState>;

    struct TrackedBody::Impl {
        osvr::util::time::TimeValue latest;
        osvr::util::time::TimeValue stateTime;

        HistoryContainer<BodyStateHistoryEntry> stateHistory;
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
    osvr::util::time::TimeValue TrackedBody::getStateTime() const {
        return m_impl->stateTime;
    }

    bool TrackedBody::getStateAtOrBefore(
        osvr::util::time::TimeValue const &desiredTime,
        osvr::util::time::TimeValue &outTime, BodyState &outState) {
        auto it = m_impl->stateHistory.closest_not_newer(desiredTime);
        if (m_impl->stateHistory.end() == it) {
            /// couldn't find such a state.
            return false;
        }
        outTime = it->first;
        it->second.restore(outState);
        return true;
    }

    inline boost::optional<osvr::util::time::TimeValue>
    getOldestPossibleMeasurementSource(TrackedBody const &body) {
        boost::optional<osvr::util::time::TimeValue> oldest;
        body.forEachTarget([&oldest](TrackedBodyTarget const &target) {
            if (!oldest || target.getLastUpdate() < *oldest) {
                /// If we haven't recorded a timestamp or the current target has
                /// an older timestamp
                oldest = target.getLastUpdate();
            }
        });
        /// @todo include IMU
        return oldest;
    }

    void TrackedBody::pruneHistory() {
        if (m_impl->stateHistory.empty()) {
            // can't prune an empty structure
            return;
        }
        auto oldestOptional = getOldestPossibleMeasurementSource(*this);
        if (!oldestOptional) {
            // No timestamp yet - don't prune anything out yet.
            return;
        }
        auto oldest = *oldestOptional;
        if (m_impl->stateHistory.newest_timestamp() < oldest) {
            // It would be a strange set of circumstances to bring this about,
            // but we don't want to go from a non-empty history to an empty one.
            // So in this case, we want to make sure we have at least one entry
            // left over in the state history. Here's a quick way of doing that.
            oldest = m_impl->stateHistory.newest_timestamp();
        }

        m_impl->stateHistory.pop_before(oldest);

        /// @todo prune IMU measurements as well.
    }

    void TrackedBody::replaceStateSnapshot(
        osvr::util::time::TimeValue const &origTime,
        osvr::util::time::TimeValue const &newTime, BodyState const &newState) {
#if !(defined(OSVR_UVBI_ASSUME_SINGLE_CAMERA) &&                               \
      defined(OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER))
#error "Current code assumes that all we have to replay is IMU measurements."
#endif // !(defined(OSVR_UVBI_ASSUME_SINGLE_CAMERA) &&
        // defined(OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER))

        auto numPopped = m_impl->stateHistory.pop_after(origTime);
        /// @todo number popped should be the same as the number of IMU
        /// measurements we replay.
        m_impl->stateHistory.push_newest(newTime,
                                         BodyStateHistoryEntry{newState});
        /// @todo replay IMU measurements.

        m_state = newState;
    }

    bool TrackedBody::hasPoseEstimate() const {
        /// @todo handle IMU here.
        auto ret = false;
        forEachTarget([&ret](TrackedBodyTarget &target) {
            ret = ret || target.hasPoseEstimate();
        });
        return ret;
    }

} // namespace vbtracker
} // namespace osvr
