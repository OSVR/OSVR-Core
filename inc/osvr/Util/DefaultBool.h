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

#ifndef INCLUDED_DefaultBool_h_GUID_3AFC9D48_7BCD_4AAD_CC67_E3FE744CF724
#define INCLUDED_DefaultBool_h_GUID_3AFC9D48_7BCD_4AAD_CC67_E3FE744CF724

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief A class template primarily useful for flags, that has a defined
    /// value at default initialization.
    template <bool DefaultValue = false> class DefaultBool {
      public:
        /// @brief Default constructor, initializes value to specified default.
        DefaultBool() : m_val(DefaultValue) {}

        /// @brief Explicit conversion to bool
        explicit operator bool() const { return m_val; }

        /// @brief Assignment from a bool
        DefaultBool &operator=(bool val) {
            m_val = val;
            return *this;
        }

        /// @brief Restore the default value
        void reset() { m_val = DefaultValue; }

      private:
        bool m_val;
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_DefaultBool_h_GUID_3AFC9D48_7BCD_4AAD_CC67_E3FE744CF724
