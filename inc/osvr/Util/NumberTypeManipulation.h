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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_NumberTypeManipulation_h_GUID_D6AC63FF_9257_488B_E06B_0B3FF403E822
#define INCLUDED_NumberTypeManipulation_h_GUID_D6AC63FF_9257_488B_E06B_0B3FF403E822

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/mpl/identity.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_signed.hpp>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace util {
    namespace detail {
        class NumberTypeDecompositionFunctor;
    } // namespace detail

    /// @brief Runtime data on numeric types.
    class NumberTypeData {
      public:
        /// @brief Get runtime data on a given numeric type, passed as the
        /// template argument.
        template <typename T> static NumberTypeData get();

        /// @brief Default constructor, for an unsigned 1-byte (8 bit) type.
        NumberTypeData() : m_signed(false), m_sizeof(1), m_float(false) {}

        /// @brief Construct a runtime number type descriptor.
        ///
        /// @param size Bytes: 1, 2, 4, or 8
        /// @param isTypeSigned true if type is a signed integer, false
        /// otherwise
        /// @param isTypeFloating true if type is a floating-point type (4 or 8
        /// bytes), false otherwise.
        NumberTypeData(size_t size, bool isTypeSigned, bool isTypeFloating)
            : m_signed(isTypeSigned), m_sizeof(size), m_float(isTypeFloating) {
            if (m_float) {
                m_signed = false;
            }
            switch (m_sizeof) {
            case 1:
            case 2:
            case 4:
            case 8:
                break;
            default:
                throw std::logic_error(
                    "Can't describe a numeric type of that size!");
            }
        }

        /// @brief is the type a signed integer?
        bool isSigned() const { return m_signed; }

        /// @brief is the type a floating-point type?
        bool isFloatingPoint() const { return m_float; }

        /// @brief Get the size in bytes of the type.
        size_t getSize() const { return m_sizeof; }

        /// @brief Calls the user-provided functor with the type described by
        /// this runtime data, wrapped in a boost::mpl::identity.
        template <typename Functor> void callFunctorWithType(Functor &f) {
            using boost::mpl::identity;
            switch (getSize()) {
            case 1:
                if (isFloatingPoint()) {
                    throw std::logic_error(
                        "Can't construct a 1-byte floating point type!");
                }
                if (isSigned()) {
                    f(identity<int8_t>());
                } else {
                    f(identity<uint8_t>());
                }
                return;
            case 2:
                if (isFloatingPoint()) {
                    throw std::logic_error(
                        "Can't construct a 2-byte floating point type!");
                }
                if (isSigned()) {
                    f(identity<int16_t>());
                } else {
                    f(identity<uint16_t>());
                }
                return;
            case 4:
                if (isFloatingPoint()) {
                    f(identity<float>());
                } else {
                    if (isSigned()) {
                        f(identity<int32_t>());
                    } else {
                        f(identity<uint32_t>());
                    }
                }
                return;
            case 8:
                if (isFloatingPoint()) {
                    f(identity<double>());
                } else {
                    if (isSigned()) {
                        f(identity<int64_t>());
                    } else {
                        f(identity<uint64_t>());
                    }
                }
                return;
            default:
                throw std::logic_error(
                    "Can't construct a numeric type of that size!");
            }
        }

      private:
        bool m_signed;
        size_t m_sizeof;
        bool m_float;
        friend class detail::NumberTypeDecompositionFunctor;
    };

    namespace detail {
        class NumberTypeDecompositionFunctor : boost::noncopyable {
          public:
            NumberTypeDecompositionFunctor(NumberTypeData &data)
                : m_data(data) {}
            template <typename WrappedT> void operator()(WrappedT const &) {
                typedef typename WrappedT::type T;
                m_data.m_sizeof = sizeof(T);
                m_data.m_float = boost::is_floating_point<T>::value;
                m_data.m_signed = boost::is_signed<T>::value;
            }

          private:
            NumberTypeData &m_data;
        };
    } // namespace detail

    template <typename T> inline NumberTypeData NumberTypeData::get() {
        NumberTypeData d;
        detail::NumberTypeDecompositionFunctor f(d);
        f(boost::mpl::identity<T>());
        return d;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_NumberTypeManipulation_h_GUID_D6AC63FF_9257_488B_E06B_0B3FF403E822
