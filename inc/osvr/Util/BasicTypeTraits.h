/** @file
    @brief Header containing some basic, C++11-aligned implementations of
   functionality provided by <type_traits> or Boost, but for use in headers
   where we can't rely on either being available. Names differ from standard to
   avoid confusion.

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

#ifndef INCLUDED_BasicTypeTraits_h_GUID_BB01797C_21D1_47B6_9B75_EAA829C6CB2E
#define INCLUDED_BasicTypeTraits_h_GUID_BB01797C_21D1_47B6_9B75_EAA829C6CB2E

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {

    /// @brief Quick C++11-aligned enable_if implementation.
    template <bool, class T = void> struct EnableIf {};
    template <class T> struct EnableIf<true, T> { typedef T type; };

    /// @brief Quick C++11-aligned conditional (if/then/elese) implementation
    template <bool, class Then, class Else> struct Conditional {
        typedef Then type;
    };
    template <class Then, class Else> struct Conditional<false, Then, Else> {
        typedef Else type;
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_BasicTypeTraits_h_GUID_BB01797C_21D1_47B6_9B75_EAA829C6CB2E
