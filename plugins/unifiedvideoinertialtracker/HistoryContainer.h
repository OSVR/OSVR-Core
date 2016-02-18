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

#ifndef INCLUDED_HistoryContainer_h_GUID_7CD22B1E_6EAC_49BF_F9FB_030C4BE3FA54
#define INCLUDED_HistoryContainer_h_GUID_7CD22B1E_6EAC_49BF_F9FB_030C4BE3FA54

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/TimeValue.h>

// Standard includes
#include <deque>
#include <stdexcept>
#include <iterator>

namespace osvr {
namespace vbtracker {
    namespace history {

        namespace detail {
            using timestamp = osvr::util::time::TimeValue;

            template <typename ValueType>
            using full_value_type = std::pair<timestamp, ValueType>;

            template <typename ValueType>
            using inner_container_type = std::deque<full_value_type<ValueType>>;

            template <typename ValueType>
            using container_size_type =
                typename inner_container_type<ValueType>::size_type;

            template <typename ValueType>
            using iterator =
                typename inner_container_type<ValueType>::const_iterator;

            template <typename ValueType>
            using nonconst_iterator =
                typename inner_container_type<ValueType>::iterator;

            /// Comparison functor for std algorithms usage with
            /// HistoryContainer and related containers.
            template <typename ValueType> class TimestampPairLessThan {
              public:
                bool operator()(timestamp const &lhs,
                                full_value_type<ValueType> const &rhs) {
                    return lhs < rhs.first;
                }

                bool operator()(full_value_type<ValueType> const &lhs,
                                timestamp const &rhs) {
                    return lhs.first < rhs;
                }
            };
            /// Convenience class to refer to a subset of the range of history,
            /// primarily for use in range-for loops. Note that all iterators
            /// are const iterators.
            template <typename ValueType> class HistorySubsetRange {
              public:
                using iterator = detail::iterator<ValueType>;
                using const_iterator = detail::iterator<ValueType>;
                HistorySubsetRange(iterator begin_, iterator end_)
                    : m_begin(begin_), m_end(end_) {
                    /// @todo consistency checks on the iterators...
                }

                iterator begin() const { return m_begin; }
                const_iterator cbegin() const { return m_begin; }
                iterator end() const { return m_end; }
                const_iterator cend() const { return m_end; }

              private:
                iterator m_begin;
                iterator m_end;
            };
        } // namespace detail

        /// Stores values over time, in chronological order, in a deque for
        /// two-ended access.
        template <typename ValueType, bool AllowDuplicateTimes_ = true>
        class HistoryContainer {
          public:
            using value_type = ValueType;

            using timestamp_type = detail::timestamp;
            using full_value_type = detail::full_value_type<value_type>;
            using container_type = detail::inner_container_type<value_type>;
            using size_type = detail::container_size_type<value_type>;

            using iterator = detail::iterator<value_type>;
            using const_iterator = iterator;

            using comparator_type = detail::TimestampPairLessThan<value_type>;

            using subset_range_type = detail::HistorySubsetRange<value_type>;

            /// Whether multiple entries with the same timestamp are permitted
            /// to be pushed.
            static const bool AllowDuplicateTimes = AllowDuplicateTimes_;

            /// Get number of entries in history.
            size_type size() const { return m_history.size(); }

            /// Gets whether history is empty or not.
            bool empty() const { return m_history.empty(); }

            timestamp_type const &oldest_timestamp() const {
                if (empty()) {
                    throw std::logic_error(
                        "Can't get time of oldest entry in an "
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

            /// Returns the newest timestamp in the container. Caveat: throws an
            /// exception in an empty container - if you want to actually
            /// compare to see if you're at least as new as the newest, see
            /// is_as_new_as_newest(), if you want to see if your value is newer
            /// than all others, see is_strictly_newest(). Both of these contain
            /// special handling for empty containers.
            timestamp_type const &newest_timestamp() const {
                if (empty()) {
                    throw std::logic_error(
                        "Can't get time of newest entry in an "
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

          private:
            /// Needed due to some pre-modern-C++ library differences (like
            /// containter_type::erase)
            using nonconst_iterator = detail::nonconst_iterator<value_type>;

            nonconst_iterator ncbegin() { return m_history.begin(); }
            nonconst_iterator ncend() { return m_history.end(); }

          public:
            /// Returns true if the given timestamp is strictly newer than the
            /// newest timestamp in the container, or if the container is empty
            /// (thus making the timestamp trivially newest)
            bool is_strictly_newest(timestamp_type const &tv) const {
                return empty() || newest_timestamp() < tv;
            }

            /// Returns true if the given timestamp is no older than the
            /// newest timestamp in the container, or if the container is empty
            /// (thus making the timestamp trivially newest)
            bool is_as_new_as_newest(timestamp_type const &tv) const {
                return empty() || !(tv < newest_timestamp());
            }

            /// Returns true if the given timestamp meets the criteria of
            /// push_newest: strictly newest if AllowDuplicateTimes is false, as
            /// new as newest if AllowDuplicateTimes is true.
            bool is_valid_to_push_newest(timestamp_type const &tv) {
                return empty() || tv > newest_timestamp() ||
                       (AllowDuplicateTimes && newest_timestamp() == tv);
            }

            /// Wrapper around std::upper_bound: returns iterator to first
            /// element newer than timestamp given or end() if none.
            const_iterator upper_bound(timestamp_type const &tv) const {
                return std::upper_bound(begin(), end(), tv, comparator());
            }
            /// Wrapper around std::lower_bound: returns iterator to first
            /// element with timestamp equal or newer than timestamp given or
            /// end() if none.
            const_iterator lower_bound(timestamp_type const &tv) const {
                return std::lower_bound(begin(), end(), tv, comparator());
            }

          private:
            /// Wrapper around std::upper_bound: returns iterator to first
            /// element newer than timestamp given or end() if none.
            ///
            /// Returns a non-const iterator, for older C++ standard library
            /// implementations that require one for erase and similar.
            nonconst_iterator nc_upper_bound(timestamp_type const &tv) {
                return std::upper_bound(ncbegin(), ncend(), tv, comparator());
            }
            /// Wrapper around std::lower_bound: returns iterator to first
            /// element with timestamp equal or newer than timestamp given or
            /// end() if none.
            ///
            /// Returns a non-const iterator, for older C++ standard library
            /// implementations that require one for erase and similar.
            nonconst_iterator nc_lower_bound(timestamp_type const &tv) {
                return std::lower_bound(ncbegin(), ncend(), tv, comparator());
            }

          public:
            /// Return an iterator to the newest, last pair of timestamp and
            /// value that is not newer than the given timestamp. If none meet
            /// this criteria, returns end().
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

            /// Returns a proxy object that can be treated as a range in a
            /// range-for loop to iterate over all elements strictly newer than
            /// the given timestamp.
            /// (Uses upper_bound internally.)
            subset_range_type
            get_range_newer_than(timestamp_type const &tv) const {
                return subset_range_type(upper_bound(tv), end());
            }

#if 0
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
#else
            /// Remove all entries in history with timestamps strictly older
            /// than the given timestamp.
            /// @return number of elements removed.
            size_type pop_before(timestamp_type const &tv) {
                if (empty()) {
                    return 0;
                }
                auto lastIt = nc_lower_bound(tv);
                if (ncend() == lastIt) {
                    // If we got end() back, that's ambiguous: is the last entry
                    // really >= our timestamp?
                    /// @todo is this right?
                    if (is_strictly_newest(tv)) {
                        // It's not - lower_bound couldn't find anything.
                        return 0;
                    }
                }
                auto count = std::distance(ncbegin(), lastIt);
                m_history.erase(ncbegin(), lastIt);
                return count;
            }
#endif

#if 0

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
#else
            /// Remove all entries in history with timestamps strictly newer
            /// than the given timestamp.
            /// @return number of elements removed.
            size_type pop_after(timestamp_type const &tv) {
                if (empty()) {
                    return 0;
                }
                auto firstIt = nc_upper_bound(tv);
                if (ncend() == firstIt) {
                    // If we got end() back, nothing found after our timestamp.
                    return 0;
                }
                auto count = std::distance(firstIt, ncend());
                m_history.erase(firstIt, ncend());
                return count;
            }
#endif

            /// Adds a new value to history. It must be newer (or equal time,
            /// based on template parameters) than the newest (or the history
            /// must be empty).
            void push_newest(osvr::util::time::TimeValue const &tv,
                             value_type const &value) {
                if (is_valid_to_push_newest(tv)) {
                    m_history.emplace_back(tv, value);
                } else {
                    throw std::logic_error(
                        "Can't push_newest a value that's older "
                        "than the most recent value!");
                }
            }

          private:
            container_type m_history;
        };
    } // namespace history

    using history::HistoryContainer;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_HistoryContainer_h_GUID_7CD22B1E_6EAC_49BF_F9FB_030C4BE3FA54
