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
#include <type_traits>

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

        static_assert(std::is_integral<ValueType>::value,
                      "Ranges only available with integral types");

        ValueOrRange()
            : m_begin(0), m_end(m_begin + 1), m_isInitialized(false),
              m_isRange(false) {}

        ValueType getMin() const {
            BOOST_ASSERT_MSG(isInitialized(),
                             "Should only call if you know it's initialized!");
            return m_begin;
        }

        ValueType getMax() const {
            BOOST_ASSERT_MSG(isInitialized(),
                             "Should only call if you know it's initialized!");
            return m_end - 1;
        }

        bool isInitialized() const { return m_isInitialized; }

        bool isRange() const { return isInitialized() && m_isRange; }

        bool isValue() const { return isInitialized() && !m_isRange; }

        value_type getValue() const {
            BOOST_ASSERT_MSG(isValue(), "Should only call getValue if you know "
                                        "it's an initialized value!");
            return m_begin;
        }

        void setValue(ValueType val) {
            m_isInitialized = true;
            m_isRange = false;
            m_begin = val;
            m_setMax(val);
        }

        ValueOrRange getIntersectionWithRangeMaxMin(ValueType maxVal,
                                                    ValueType minVal = 0) {
            if (!isInitialized()) {
                throw std::logic_error(
                    "Can't intersect with a non-initialized range!");
            }
            ValueOrRange ret;
            ret.setRangeMaxMin(std::min(getMax(), maxVal),
                               std::max(getMin(), minVal));
            return ret;
        }

        void setRangeMaxMin(ValueType maxVal, ValueType minVal = 0) {
            if (maxVal == minVal) {
                setValue(maxVal);
                return;
            }
            if (minVal > maxVal) {
                throw std::runtime_error("Can't have a minimum value larger "
                                         "than your maximum value!");
            }

            m_isInitialized = true;
            m_isRange = true;
            m_begin = minVal;
            m_setMax(maxVal);
        }

        void extendRangeToMax(ValueType maxVal) {
            if (!isInitialized()) {
                throw std::logic_error("Can't extend a non-initialized range!");
            }
            if (!isRange()) {
                throw std::logic_error("Can't extend a value as a range!");
            }
            if (maxVal >= m_end) {
                m_setMax(maxVal);
            }
        }

        const_iterator begin() const {
            BOOST_ASSERT_MSG(isInitialized(), "Should only call begin() if you "
                                              "know your value or range is "
                                              "initialized!");
            return const_iterator(m_begin);
        }

        const_iterator end() const {
            BOOST_ASSERT_MSG(isInitialized(), "Should only call end() if you "
                                              "know your value or range is "
                                              "initialized!");
            return const_iterator(m_end);
        }

      private:
        void m_setMax(ValueType maxVal) {
            /// @todo check for overflow potential before incrementing and
            /// setting
            m_end = maxVal + 1;
        }
        ValueType m_begin;
        ValueType m_end;
        bool m_isInitialized;
        bool m_isRange;
    };
} // end namespace util

} // end namespace osvr

#endif // INCLUDED_ValueOrRange_h_GUID_CAAC3116_82B6_4FB9_D32D_BC3391D2D072
