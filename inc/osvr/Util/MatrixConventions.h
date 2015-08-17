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

#ifndef INCLUDED_MatrixConventions_h_GUID_82404AE2_0D13_46A4_4618_05AD5A20D7EA
#define INCLUDED_MatrixConventions_h_GUID_82404AE2_0D13_46A4_4618_05AD5A20D7EA

// Internal Includes
#include <osvr/Util/MatrixConventionsC.h>

// Library/third-party includes
// - none

// Standard includes
#include <bitset>
#include <type_traits>

namespace osvr {
namespace util {
    namespace detail {

        enum class CompactMatrixFlags {
            /// @brief Set when we need to transpose from internally, where
            /// we have col-major matrices and col-vectors
            NeedsTranspose = 0,
            LeftHandInput = 1,
            UnsignedZ = 2,

            SIZEPLUSONE
        };

        inline bool matrixNeedsTranspose(OSVR_MatrixConventions flags) {
            const bool rowmaj(0 != (flags & OSVR_MATRIX_MASK_ROWMAJOR));
            const bool rowvec(0 != (flags & OSVR_MATRIX_MASK_ROWVECTORS));
            // xor since one alone implies transpose, but both together
            // imply no transpose.
            return (rowmaj ^ rowvec);
        }
        class CompactMatrixConventions {

          public:
            explicit CompactMatrixConventions(OSVR_MatrixConventions flags)
                : m_data() {
                if (matrixNeedsTranspose(flags)) {
                    set(CompactMatrixFlags::NeedsTranspose);
                }

                if (0 != (flags & OSVR_MATRIX_MASK_LHINPUT)) {
                    set(CompactMatrixFlags::LeftHandInput);
                }

                if (0 != (flags & OSVR_MATRIX_MASK_UNSIGNEDZ)) {
                    set(CompactMatrixFlags::UnsignedZ);
                }
            }
            template <typename... Args>
            CompactMatrixConventions(Args... args)
                : m_data() {
                set(args...);
            }

            bool operator[](CompactMatrixFlags flag) const {
                return m_data[static_cast<std::size_t>(flag)];
            }

            unsigned long getValue() const { return m_data.to_ulong(); }

            void set(CompactMatrixFlags flag) {
                m_data.set(static_cast<std::size_t>(flag));
            }

            template <typename... Args>
            void set(CompactMatrixFlags flag, Args... args) {
                set(flag);
                set(args...);
            }

          private:
            void set() {}
            static const std::size_t SIZE =
                static_cast<std::size_t>(CompactMatrixFlags::SIZEPLUSONE) - 1;
            std::bitset<SIZE> m_data;

            template <CompactMatrixFlags Flag>
            using FlagToMask =
                std::integral_constant<std::size_t,
                                       (0x1 << static_cast<std::size_t>(Flag))>;

            // General template declaration
            template <std::size_t State, CompactMatrixFlags... MyFlags>
            struct ComputeBitsImpl;
            // Base case
            template <std::size_t State>
            struct ComputeBitsImpl<State>
                : std::integral_constant<std::size_t, State> {};
            // Recursive case
            template <std::size_t State, CompactMatrixFlags Flag,
                      CompactMatrixFlags... MyFlags>
            struct ComputeBitsImpl<State, Flag, MyFlags...>
                : ComputeBitsImpl<(State | FlagToMask<Flag>::value),
                                  MyFlags...> {};

          public:
            template <CompactMatrixFlags... MyFlags>
            using ComputeBits = ComputeBitsImpl<0, MyFlags...>;
        };

    } // namespace detail
} // namespace util
} // namespace osvr
#endif // INCLUDED_MatrixConventions_h_GUID_82404AE2_0D13_46A4_4618_05AD5A20D7EA
