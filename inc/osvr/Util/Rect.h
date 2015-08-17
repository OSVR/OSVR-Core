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

#ifndef INCLUDED_Rect_h_GUID_A81C8929_371D_4EBC_C6BB_A7264EF642E6
#define INCLUDED_Rect_h_GUID_A81C8929_371D_4EBC_C6BB_A7264EF642E6

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <array>

namespace osvr {
namespace util {
    template <typename Scalar = double> class Rect {
      public:
        typedef Rect type;
        typedef Scalar value_type;
        enum Side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3 };

        /// @brief Access by side
        value_type &operator[](Side s) {
            return m_data[static_cast<index_type>(s)];
        }
        value_type operator[](Side s) const {
            return m_data[static_cast<index_type>(s)];
        }

        /// @brief Chained function call operator for setting sides.
        type &operator()(Side s, value_type v) {
            (*this)[s] = v;
            return *this;
        }

        /// @brief Componentwise multiplication by scalar
        type &operator*=(Scalar v) {
            for (auto &e : m_data) {
                e *= v;
            }
            return *this;
        }

      private:
        typedef std::array<Scalar, 4> storage_type;
        typedef std::size_t index_type;
        storage_type m_data;
    };
    typedef Rect<> Rectd;

    template <typename StreamType, typename Scalar>
    StreamType &operator<<(StreamType &os, Rect<Scalar> const &rect) {
        typedef Rect<Scalar> R;
        os << "L: " << rect[R::LEFT];
        os << " R: " << rect[R::RIGHT];
        os << " T: " << rect[R::TOP];
        os << " B: " << rect[R::BOTTOM];
        return os;
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_Rect_h_GUID_A81C8929_371D_4EBC_C6BB_A7264EF642E6
