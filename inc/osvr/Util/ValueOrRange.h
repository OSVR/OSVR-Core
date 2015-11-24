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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ValueOrRange_h_GUID_CAAC3116_82B6_4FB9_D32D_BC3391D2D072
#define INCLUDED_ValueOrRange_h_GUID_CAAC3116_82B6_4FB9_D32D_BC3391D2D072

// Internal Includes
// - none

// Library/third-party includes
#include <boost/iterator/iterator_facade.hpp>
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace osvr {

namespace util {
    namespace detail {
        template <typename ValueType>
        class ValueOrRangeIterator
            : public boost::iterator_facade<
                  ValueOrRangeIterator<ValueType>, ValueType const,
                  boost::forward_traversal_tag, ValueType const> {
          public:
            ValueOrRangeIterator() = delete;
            explicit ValueOrRangeIterator(ValueType initial)
                : m_value(initial) {}

          private:
            friend class boost::iterator_core_access;
            void increment() { m_value++; }

            bool equal(ValueOrRangeIterator const &other) const {
                return this->m_value == other.m_value;
            }

            ValueType const dereference() const { return m_value; }
            ValueType m_value;
        };
    } // namespace detail

    /// @brief Class providing a unified container-like interface to either a
    /// single value or a range of integers.
    template <typename ValueType> class ValueOrRange {
      public:
        typedef ValueType value_type;
        typedef detail::ValueOrRangeIterator<ValueType> const_iterator;
        typedef const_iterator iterator;

        typedef ValueType size_type;

        static_assert(std::is_integral<ValueType>::value,
                      "Ranges only available with integral types");

        /// @brief Default constructor: an empty range
        ValueOrRange() : m_begin(0), m_end(m_begin) {}

        /// @brief Factory method for creating a single-value range
        static ValueOrRange SingleValue(ValueType val) {
            return ValueOrRange(val);
        }
        /// @brief Factory method for creating a range from zero
        static ValueOrRange RangeZeroTo(ValueType maxVal) {
            return ValueOrRange(maxVal, 0);
        }

        /// @brief Factory method for creating a range from zero
        static ValueOrRange RangeMaxMin(ValueType maxVal, ValueType minVal) {
            return ValueOrRange(maxVal, minVal);
        }

        /// @brief Determine if the range contains the given value (treating a
        /// value instance of this object as essentially a single-element range)
        template <typename T> bool contains(T val) const {
            static_assert(std::is_integral<T>::value,
                          "Ranges only available with integral types");
            return !empty() && getMin() <= val && val <= getMax();
        }

        /// @brief Get minimum value - closed lower bound
        ///
        /// Only valid if not empty()!
        ValueType getMin() const {
            BOOST_ASSERT_MSG(!empty(), "Should only call if not empty!");
            return m_begin;
        }

        /// @brief Get maximum value - closed upper bound
        ///
        /// Only valid if not empty()!
        ValueType getMax() const {
            BOOST_ASSERT_MSG(!empty(), "Should only call if not empty!");
            return m_end - 1;
        }

        /// @brief Has the object been assigned a single value (range of size 1)
        bool isValue() const { return (m_begin + 1) == m_end; }

        /// @brief Has the object been assigned a range of size > 1?
        bool isNonEmptyRange() const { return m_end > (m_begin + 1); }

        /// @brief Is the object an empty range? (default constructor, or
        /// setting to a range with max < min)
        bool empty() const { return m_begin == 0 && m_end == 0; }

        size_type size() const { return m_end - m_begin; }

        value_type getValue() const {
            BOOST_ASSERT_MSG(isValue(), "Should only call getValue if you know "
                                        "it's an initialized value!");
            return m_begin;
        }

        /// @brief Assign a single value to this object.
        void setValue(ValueType val) {
            m_begin = val;
            m_setMax(val);
        }

        /// @brief Assign an empty range to this object
        void setEmpty() {
            m_begin = 0;
            m_end = 0;
        }

        /// @brief Assign a range of values to this object.
        ///
        /// Note the order of parameters to make minimum (0) optional.
        ///
        /// Giving a range of size == 1 is the same as calling setValue().
        /// Giving a range where min is greater than max sets the range to be
        /// empty, discarding your provided max and min values for sentinel
        /// values.
        void setRangeMaxMin(ValueType maxVal, ValueType minVal = 0) {
            if (minVal > maxVal) {
                setEmpty();
            }
            m_begin = minVal;
            m_setMax(maxVal);
        }

        /// @brief Gets the (possibly empty) intersection of the ranges/values
        ValueOrRange getIntersection(ValueOrRange other) const {
            if (other.empty()) {
                return other;
            }
            if (empty()) {
                other.setEmpty();
                return other;
            }
            auto newMax = std::min(this->getMax(), other.getMax());
            auto newMin = std::max(this->getMin(), other.getMin());
            other.setRangeMaxMin(newMax, newMin);
            return other;
        }

        /// @brief If this is an initialized range, extend it as needed such
        /// that the given maxVal is included.
        void extendRangeToMax(ValueType maxVal) {
            if (empty()) {
                throw std::logic_error(
                    "Can't extend an empty range with a new max!");
            }
            if (maxVal >= m_end) {
                m_setMax(maxVal);
            }
        }

        /// @brief Get a "begin" iterator pointing to the first value in the
        /// range (or value)
        const_iterator begin() const { return const_iterator(m_begin); }

        /// @brief Get a "past-the-end" iterator pointing to one more than the
        /// max value)
        const_iterator end() const { return const_iterator(m_end); }

      private:
        /// @brief Single value private constructor
        explicit ValueOrRange(ValueType val) : m_begin(val) { m_setMax(val); }

        /// @brief Range private constructor
        ValueOrRange(ValueType maxVal, ValueType minVal) : m_begin(minVal) {
            if (maxVal >= minVal) {
                m_setMax(maxVal);
            } else {
                setEmpty();
            }
        }

        void m_setMax(ValueType maxVal) {
            /// @todo check for overflow potential before incrementing and
            /// setting
            m_end = maxVal + 1;
        }
        ValueType m_begin;
        ValueType m_end;
        bool m_isInitialized;
    };
} // end namespace util

} // end namespace osvr

#endif // INCLUDED_ValueOrRange_h_GUID_CAAC3116_82B6_4FB9_D32D_BC3391D2D072
