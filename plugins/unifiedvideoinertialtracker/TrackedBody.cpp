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

// Library/third-party includes
#include <osvr/Kalman/FlexibleKalmanFilter.h>

// Standard includes
#include <iostream>
#include <deque>

namespace osvr {
namespace vbtracker {
    using BodyStateHistoryEntry = StateHistoryEntry<BodyState>;

    namespace detail {
        template <typename ValueType> class TimestampPairLessThan {
          public:
            using timestamp = osvr::util::time::TimeValue;
            using pair_type = std::pair<timestamp, ValueType>;

            bool operator()(timestamp const &lhs, pair_type const &rhs) {
                return lhs < rhs.first;
            }

            bool operator()(pair_type const &lhs, timestamp const &rhs) {
                return lhs.first < rhs;
            }
        };
    } // namespace detail
    /// Stores values over time, in chronological order, in a deque for
    /// two-ended access.
    template <typename ValueType, bool AllowDuplicateTimes = true>
    class HistoryContainer {
      public:
        using value_type = ValueType;

        using timestamp_type = osvr::util::time::TimeValue;
        using full_value_type = std::pair<timestamp_type, value_type>;
        using container_type = std::deque<full_value_type>;
        using size_type = typename container_type::size_type;

        using iterator = typename container_type::const_iterator;
        using const_iterator = typename container_type::const_iterator;

        using comparator_type = detail::TimestampPairLessThan<value_type>;

        /// Get number of entries in history.
        size_type size() const { return m_history.size(); }

        /// Gets whether history is empty or not.
        bool empty() const { return m_history.empty(); }

        timestamp_type const &oldest_timestamp() const {
            if (empty()) {
                throw std::logic_error("Can't get time of oldest entry in an "
                                       "empty history container!");
            }
            return m_history.front().first;
        }

        value_type const &oldest() const {
            if (empty()) {
                throw std::logic_error("Can't get oldest entry in an "
                                       "empty history container!");
            }
            return m_history.front().first;
        }

        timestamp_type const &newest_timestamp() const {
            if (empty()) {
                throw std::logic_error("Can't get time of newest entry in an "
                                       "empty history container!");
            }

            return m_history.back().first;
        }

        value_type const &newest() const {
            if (empty()) {
                throw std::logic_error("Can't get newest entry in an "
                                       "empty history container!");
            }

            return m_history.back().second;
        }

        /// Returns a comparison functor (comparing timestamps) for use with
        /// standard algorithms like lower_bound and upper_bound
        static comparator_type comparator() { return comparator_type{}; }

        void pop_oldest() { m_history.pop_front(); }
        void pop_newest() { m_history.pop_back(); }

        const_iterator begin() const { return m_history.cbegin(); }
        const_iterator cbegin() const { return m_history.cbegin(); }
        const_iterator end() const { return m_history.cend(); }
        const_iterator cend() const { return m_history.cend(); }

        /// Wrapper around std::upper_bound: returns iterator to first element
        /// newer than timestamp given or end() if none.
        const_iterator upper_bound(timestamp_type const &tv) const {
            return std::upper_bound(begin(), end(), tv, comparator());
        }

        /// Return an iterator to the newest, last pair of timestamp and value
        /// that is not newer than the given timestamp. If none meet this
        /// criteria, returns end().
        const_iterator closest_not_newer(timestamp_type const &tv) const {
            if (empty()) {
                return end();
            }
            /// This will return one element past where we want. It's OK if
            /// that's end(), but not if it's begin().
            auto it = upper_bound(tv);
            if (begin() == it) {
                // can't back iterator off if it's already at the beginning
                return end();
            }
            --it;
            return it;
        }

        /// Remove all entries in history with timestamps strictly older than
        /// the given timestamp.
        /// @return number of elements removed.
        size_type pop_before(timestamp_type const &tv) {
            auto count = size_type{0};
            while (!empty() && oldest_timestamp() < tv) {
                pop_oldest();
                ++count;
            }
            return count;
        }

        /// Remove all entries in history with timestamps strictly newer than
        /// the given timestamp.
        /// @return number of elements removed.
        size_type pop_after(timestamp_type const &tv) {
            auto count = size_type{0};
            while (!empty() && tv < newest_timestamp()) {
                pop_newest();
                ++count;
            }
            return count;
        }

        /// Adds a new value to history. It must be newer (or equal time, based
        /// on template parameters) than the newest (or the history must be
        /// empty).
        void push_newest(osvr::util::time::TimeValue const &tv,
                         value_type const &value) {
            if (empty() || tv > newest_timestamp() ||
                (tv == newest_timestamp() && AllowDuplicateTimes)) {
                m_history.emplace_back(tv, value);
            } else {
                throw std::logic_error("Can't push_newest a value that's older "
                                       "than the most recent value!");
            }
        }

      private:
        container_type m_history;
    };

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
#if 0
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
#endif
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

    void TrackedBody::replaceStateSnapshot(
        osvr::util::time::TimeValue const &origTime,
        osvr::util::time::TimeValue const &newTime, BodyState const &newState) {
#if !(defined(OSVR_UVBI_ASSUME_SINGLE_CAMERA) &&                               \
      defined(OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER))
#error "Current code assumes that all we have to replay is IMU measurements."
#endif  // !(defined(OSVR_UVBI_ASSUME_SINGLE_CAMERA) &&
        // defined(OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER))

        auto numPopped = m_impl->stateHistory.pop_after(origTime);
        /// @todo number popped should be the same as the number of IMU
        /// measurements we replay.
        m_impl->stateHistory.push_newest(newTime,
                                         BodyStateHistoryEntry{newState});
        /// @todo replay IMU measurements.
    }

} // namespace vbtracker
} // namespace osvr
